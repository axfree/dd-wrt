/*
 * Copyright (C) 2013 Martin Willi
 * Copyright (C) 2013 revosec AG
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "eap_radius_provider.h"

#include <daemon.h>
#include <collections/hashtable.h>
#include <threading/mutex.h>

typedef struct private_eap_radius_provider_t private_eap_radius_provider_t;
typedef struct private_listener_t private_listener_t;

/**
 * Private data of registered listener
 */
struct private_listener_t {

	/**
	 * Implements listener_t interface
	 */
	listener_t public;

	/**
	 * Leases not acquired yet, identification_t => entry_t
	 */
	hashtable_t *unclaimed;

	/**
	 * Leases acquired, identification_t => entry_t
	 */
	hashtable_t *claimed;

	/**
	 * Mutex to lock leases
	 */
	mutex_t *mutex;
};

/**
 * Private data of an eap_radius_provider_t object.
 */
struct private_eap_radius_provider_t {

	/**
	 * Public eap_radius_provider_t interface.
	 */
	eap_radius_provider_t public;

	/**
	 * Additionally implements the listener_t interface
	 */
	private_listener_t listener;
};

/**
 * Singleton instance of provider
 */
static eap_radius_provider_t *singleton = NULL;

/**
 * Configuration attribute in an entry
 */
typedef struct {
	/** type of attribute */
	configuration_attribute_type_t type;
	/** attribute data */
	chunk_t data;
} attr_t;

/**
 * Destroy an attr_t
 */
static void destroy_attr(attr_t *this)
{
	free(this->data.ptr);
	free(this);
}

/**
 * Hashtable entry with leases and attributes
 */
typedef struct {
	/** identity we assigned the IP lease */
	identification_t *id;
	/** list of IP leases received from AAA, as host_t */
	linked_list_t *addrs;
	/** list of configuration attributes, as attr_t */
	linked_list_t *attrs;
} entry_t;

/**
 * destroy an entry_t
 */
static void destroy_entry(entry_t *this)
{
	this->id->destroy(this->id);
	this->addrs->destroy_offset(this->addrs, offsetof(host_t, destroy));
	this->attrs->destroy_function(this->attrs, (void*)destroy_attr);
	free(this);
}

/**
 * Get or create an entry from a locked hashtable
 */
static entry_t* get_or_create_entry(hashtable_t *hashtable, identification_t *id)
{
	entry_t *entry;

	entry = hashtable->get(hashtable, id);
	if (!entry)
	{
		INIT(entry,
			.id = id->clone(id),
			.addrs = linked_list_create(),
			.attrs = linked_list_create(),
		);
		hashtable->put(hashtable, entry->id, entry);
	}
	return entry;
}

/**
 * Put an entry to hashtable, or destroy it ife empty
 */
static void put_or_destroy_entry(hashtable_t *hashtable, entry_t *entry)
{
	if (entry->addrs->get_count(entry->addrs) > 0 ||
		entry->attrs->get_count(entry->attrs) > 0)
	{
		hashtable->put(hashtable, entry->id, entry);
	}
	else
	{
		destroy_entry(entry);
	}
}

/**
 * Hashtable hash function
 */
static u_int hash(identification_t *id)
{
	return chunk_hash_inc(id->get_encoding(id), id->get_type(id));
}

/**
 * Hashtable equals function
 */
static bool equals(identification_t *a, identification_t *b)
{
	return a->equals(a, b);
}

/**
 * Insert an address entry to a locked claimed/unclaimed hashtable
 */
static void add_addr(private_eap_radius_provider_t *this,
					 hashtable_t *hashtable, identification_t *id, host_t *host)
{
	entry_t *entry;

	entry = get_or_create_entry(hashtable, id);
	entry->addrs->insert_last(entry->addrs, host);
}

/**
 * Remove the next address from the locked hashtable stored for given id
 */
static host_t* remove_addr(private_eap_radius_provider_t *this,
						   hashtable_t *hashtable, identification_t *id)
{
	entry_t *entry;
	host_t *addr = NULL;

	entry = hashtable->remove(hashtable, id);
	if (entry)
	{
		entry->addrs->remove_first(entry->addrs, (void**)&addr);
		put_or_destroy_entry(hashtable, entry);
	}
	return addr;
}

/**
 * Insert an attribute entry to a locked claimed/unclaimed hashtable
 */
static void add_attr(private_eap_radius_provider_t *this,
					 hashtable_t *hashtable, identification_t *id, attr_t *attr)
{
	entry_t *entry;

	entry = get_or_create_entry(hashtable, id);
	entry->attrs->insert_last(entry->attrs, attr);
}

/**
 * Remove the next attribute from the locked hashtable stored for given id
 */
static attr_t* remove_attr(private_eap_radius_provider_t *this,
						   hashtable_t *hashtable, identification_t *id)
{
	entry_t *entry;
	attr_t *attr = NULL;

	entry = hashtable->remove(hashtable, id);
	if (entry)
	{
		entry->attrs->remove_first(entry->attrs, (void**)&attr);
		put_or_destroy_entry(hashtable, entry);
	}
	return attr;
}

/**
 * Clean up unclaimed leases assigned for an IKE_SA
 */
static void release_unclaimed(private_listener_t *this, ike_sa_t *ike_sa)
{
	identification_t *id;
	entry_t *entry;

	id = ike_sa->get_other_eap_id(ike_sa);
	this->mutex->lock(this->mutex);
	entry = this->unclaimed->remove(this->unclaimed, id);
	this->mutex->unlock(this->mutex);
	if (entry)
	{
		destroy_entry(entry);
	}
}

METHOD(listener_t, message_hook, bool,
	private_listener_t *this, ike_sa_t *ike_sa,
	message_t *message, bool incoming, bool plain)
{
	if (plain && ike_sa->get_state(ike_sa) == IKE_ESTABLISHED &&
		!incoming && !message->get_request(message))
	{
		if ((ike_sa->get_version(ike_sa) == IKEV1 &&
			 message->get_exchange_type(message) == TRANSACTION) ||
			(ike_sa->get_version(ike_sa) == IKEV2 &&
			 message->get_exchange_type(message) == IKE_AUTH))
		{
			/* if the addresses have not been claimed yet, they won't. Release
			 * these resources. */
			release_unclaimed(this, ike_sa);
		}
	}
	return TRUE;
}

METHOD(listener_t, ike_updown, bool,
	private_listener_t *this, ike_sa_t *ike_sa, bool up)
{
	if (!up)
	{
		/* if the message hook does not apply because of a failed exchange
		 * or something, make sure we release any resources now */
		release_unclaimed(this, ike_sa);
	}
	return TRUE;
}

METHOD(attribute_provider_t, acquire_address, host_t*,
	private_eap_radius_provider_t *this, linked_list_t *pools,
	identification_t *id, host_t *requested)
{
	enumerator_t *enumerator;
	host_t *addr = NULL;
	char *name;

	enumerator = pools->create_enumerator(pools);
	while (enumerator->enumerate(enumerator, &name))
	{
		if (streq(name, "radius"))
		{
			this->listener.mutex->lock(this->listener.mutex);
			addr = remove_addr(this, this->listener.unclaimed, id);
			if (addr)
			{
				add_addr(this, this->listener.claimed, id, addr->clone(addr));
			}
			this->listener.mutex->unlock(this->listener.mutex);
			break;
		}
	}
	enumerator->destroy(enumerator);

	return addr;
}

METHOD(attribute_provider_t, release_address, bool,
	private_eap_radius_provider_t *this, linked_list_t *pools, host_t *address,
	identification_t *id)
{
	enumerator_t *enumerator;
	host_t *found = NULL;
	char *name;

	enumerator = pools->create_enumerator(pools);
	while (enumerator->enumerate(enumerator, &name))
	{
		if (streq(name, "radius"))
		{
			this->listener.mutex->lock(this->listener.mutex);
			found = remove_addr(this, this->listener.claimed, id);
			this->listener.mutex->unlock(this->listener.mutex);
			break;
		}
	}
	enumerator->destroy(enumerator);

	if (found)
	{
		found->destroy(found);
		return TRUE;
	}
	return FALSE;
}

/**
 * Enumerator implementation over attributes
 */
typedef struct {
	/** implements enumerator_t */
	enumerator_t public;
	/** list of attributes to enumerate */
	linked_list_t *list;
	/** currently enumerating attribute */
	attr_t *current;
} attribute_enumerator_t;


METHOD(enumerator_t, attribute_enumerate, bool,
	attribute_enumerator_t *this, configuration_attribute_type_t *type,
	chunk_t *data)
{
	if (this->current)
	{
		destroy_attr(this->current);
		this->current = NULL;
	}
	if (this->list->remove_first(this->list, (void**)&this->current) == SUCCESS)
	{
		*type = this->current->type;
		*data = this->current->data;
		return TRUE;
	}
	return FALSE;
}

METHOD(enumerator_t, attribute_destroy, void,
	attribute_enumerator_t *this)
{
	if (this->current)
	{
		destroy_attr(this->current);
	}
	this->list->destroy_function(this->list, (void*)destroy_attr);
	free(this);
}

METHOD(attribute_provider_t, create_attribute_enumerator, enumerator_t*,
	private_eap_radius_provider_t *this, linked_list_t *pools,
	identification_t *id, linked_list_t *vips)
{
	attribute_enumerator_t *enumerator;
	attr_t *attr;

	INIT(enumerator,
		.public = {
			.enumerate = (void*)_attribute_enumerate,
			.destroy = _attribute_destroy,
		},
		.list = linked_list_create(),
	);

	/* we forward attributes regardless of pool configurations */
	this->listener.mutex->lock(this->listener.mutex);
	while (TRUE)
	{
		attr = remove_attr(this, this->listener.unclaimed, id);
		if (!attr)
		{
			break;
		}
		enumerator->list->insert_last(enumerator->list, attr);
	}
	this->listener.mutex->unlock(this->listener.mutex);

	return &enumerator->public;
}

METHOD(eap_radius_provider_t, add_framed_ip, void,
	private_eap_radius_provider_t *this, identification_t *id, host_t *ip)
{
	this->listener.mutex->lock(this->listener.mutex);
	add_addr(this, this->listener.unclaimed, id, ip);
	this->listener.mutex->unlock(this->listener.mutex);
}

METHOD(eap_radius_provider_t, add_attribute, void,
	private_eap_radius_provider_t *this, identification_t *id,
	configuration_attribute_type_t type, chunk_t data)
{
	attr_t *attr;

	INIT(attr,
		.type = type,
		.data = chunk_clone(data),
	);
	this->listener.mutex->lock(this->listener.mutex);
	add_attr(this, this->listener.unclaimed, id, attr);
	this->listener.mutex->unlock(this->listener.mutex);
}

METHOD(eap_radius_provider_t, destroy, void,
	private_eap_radius_provider_t *this)
{
	singleton = NULL;
	charon->bus->remove_listener(charon->bus, &this->listener.public);
	this->listener.mutex->destroy(this->listener.mutex);
	this->listener.claimed->destroy(this->listener.claimed);
	this->listener.unclaimed->destroy(this->listener.unclaimed);
	free(this);
}

/**
 * See header
 */
eap_radius_provider_t *eap_radius_provider_create()
{
	if (!singleton)
	{
		private_eap_radius_provider_t *this;

		INIT(this,
			.public = {
				.provider = {
					.acquire_address = _acquire_address,
					.release_address = _release_address,
					.create_attribute_enumerator = _create_attribute_enumerator,
				},
				.add_framed_ip = _add_framed_ip,
				.add_attribute = _add_attribute,
				.destroy = _destroy,
			},
			.listener = {
				.public = {
					.ike_updown = _ike_updown,
					.message = _message_hook,
				},
				.claimed = hashtable_create((hashtable_hash_t)hash,
										(hashtable_equals_t)equals, 32),
				.unclaimed = hashtable_create((hashtable_hash_t)hash,
										(hashtable_equals_t)equals, 32),
				.mutex = mutex_create(MUTEX_TYPE_DEFAULT),
			},
		);

		charon->bus->add_listener(charon->bus, &this->listener.public);

		singleton = &this->public;
	}
	return singleton;
}

/**
 * See header
 */
eap_radius_provider_t *eap_radius_provider_get()
{
	return singleton;
}
