/*
 * Copyright (C) 2011-2013 Andreas Steffen
 * HSR Hochschule fuer Technik Rapperswil
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

#include "imv_scanner_state.h"
#include "imv/imv_lang_string.h"
#include "imv/imv_reason_string.h"
#include "imv/imv_remediation_string.h"

#include <utils/lexparser.h>
#include <utils/debug.h>

typedef struct private_imv_scanner_state_t private_imv_scanner_state_t;

/**
 * Private data of an imv_scanner_state_t object.
 */
struct private_imv_scanner_state_t {

	/**
	 * Public members of imv_scanner_state_t
	 */
	imv_scanner_state_t public;

	/**
	 * TNCCS connection ID
	 */
	TNC_ConnectionID connection_id;

	/**
	 * TNCCS connection state
	 */
	TNC_ConnectionState state;

	/**
	 * Does the TNCCS connection support long message types?
	 */
	bool has_long;

	/**
	 * Does the TNCCS connection support exclusive delivery?
	 */
	bool has_excl;

	/**
	 * Maximum PA-TNC message size for this TNCCS connection
	 */
	u_int32_t max_msg_len;

	/**
	 * Access Requestor ID Type
	 */
	u_int32_t ar_id_type;

	/**
	 * Access Requestor ID Value
	 */
	chunk_t ar_id_value;

	/**
	 * IMV action recommendation
	 */
	TNC_IMV_Action_Recommendation rec;

	/**
	 * IMV evaluation result
	 */
	TNC_IMV_Evaluation_Result eval;

	/**
	 * List with ports that should be closed
	 */
	 linked_list_t *violating_ports;

	/**
	 * TNC Reason String
	 */
	imv_reason_string_t *reason_string;

	/**
	 * IETF Remediation Instructions String
	 */
	imv_remediation_string_t *remediation_string;

};

/**
 * Supported languages
 */
static char* languages[] = { "en", "de", "fr", "pl" };

/**
 * Reason strings for "Port Filter"
 */
static imv_lang_string_t reasons[] = {
	{ "en", "Open server ports were detected" },
	{ "de", "Offene Serverports wurden festgestellt" },
	{ "fr", "Il y a des ports du serveur ouverts" },
	{ "pl", "Są otwarte porty serwera" },
	{ NULL, NULL }
};

/**
 * Instruction strings for "Port Filters"
 */
static imv_lang_string_t instr_ports_title[] = {
	{ "en", "Open Server Ports" },
	{ "de", "Offene Server Ports" },
	{ "fr", "Ports ouverts du serveur" },
	{ "pl", "Otwarte Porty Serwera" },
	{ NULL, NULL }
};

static imv_lang_string_t instr_ports_descr[] = {
	{ "en", "Open Internet ports have been detected" },
	{ "de", "Offenen Internet-Ports wurden festgestellt" },
	{ "fr", "Il y'a des ports Internet ouverts" },
	{ "pl", "Porty internetowe są otwarte" },
	{ NULL, NULL }
};

static imv_lang_string_t instr_ports_header[] = {
	{ "en", "Please close the following server ports:" },
	{ "de", "Bitte schliessen Sie die folgenden Serverports:" },
	{ "fr", "Fermez les ports du serveur suivants s'il vous plait:" },
	{ "pl", "Proszę zamknąć następujące porty serwera:" },
	{ NULL, NULL }
};

METHOD(imv_state_t, get_connection_id, TNC_ConnectionID,
	private_imv_scanner_state_t *this)
{
	return this->connection_id;
}

METHOD(imv_state_t, has_long, bool,
	private_imv_scanner_state_t *this)
{
	return this->has_long;
}

METHOD(imv_state_t, has_excl, bool,
	private_imv_scanner_state_t *this)
{
	return this->has_excl;
}

METHOD(imv_state_t, set_flags, void,
	private_imv_scanner_state_t *this, bool has_long, bool has_excl)
{
	this->has_long = has_long;
	this->has_excl = has_excl;
}

METHOD(imv_state_t, set_max_msg_len, void,
	private_imv_scanner_state_t *this, u_int32_t max_msg_len)
{
	this->max_msg_len = max_msg_len;
}

METHOD(imv_state_t, get_max_msg_len, u_int32_t,
	private_imv_scanner_state_t *this)
{
	return this->max_msg_len;
}

METHOD(imv_state_t, set_ar_id, void,
	private_imv_scanner_state_t *this, u_int32_t id_type, chunk_t id_value)
{
	this->ar_id_type = id_type;
	this->ar_id_value = chunk_clone(id_value);
}

METHOD(imv_state_t, get_ar_id, chunk_t,
	private_imv_scanner_state_t *this, u_int32_t *id_type)
{
	if (id_type)
	{
		*id_type = this->ar_id_type;
	}
	return this->ar_id_value;
}

METHOD(imv_state_t, change_state, void,
	private_imv_scanner_state_t *this, TNC_ConnectionState new_state)
{
	this->state = new_state;
}

METHOD(imv_state_t, get_recommendation, void,
	private_imv_scanner_state_t *this, TNC_IMV_Action_Recommendation *rec,
									TNC_IMV_Evaluation_Result *eval)
{
	*rec = this->rec;
	*eval = this->eval;
}

METHOD(imv_state_t, set_recommendation, void,
	private_imv_scanner_state_t *this, TNC_IMV_Action_Recommendation rec,
									TNC_IMV_Evaluation_Result eval)
{
	this->rec = rec;
	this->eval = eval;
}

METHOD(imv_state_t, get_reason_string, bool,
	private_imv_scanner_state_t *this, enumerator_t *language_enumerator,
	chunk_t *reason_string, char **reason_language)
{
	if (!this->violating_ports)
	{
		return FALSE;
	}
	*reason_language = imv_lang_string_select_lang(language_enumerator,
											  languages, countof(languages));

	/* Instantiate a TNC Reason String object */
	DESTROY_IF(this->reason_string);
	this->reason_string = imv_reason_string_create(*reason_language);
	this->reason_string->add_reason(this->reason_string, reasons);
	*reason_string = this->reason_string->get_encoding(this->reason_string);

	return TRUE;
}

METHOD(imv_state_t, get_remediation_instructions, bool,
	private_imv_scanner_state_t *this, enumerator_t *language_enumerator,
	chunk_t *string, char **lang_code, char **uri)
{
	if (!this->violating_ports)
	{
		return FALSE;
	}
	*lang_code = imv_lang_string_select_lang(language_enumerator,
										languages, countof(languages));

	/* Instantiate an IETF Remediation Instructions String object */
	DESTROY_IF(this->remediation_string);
	this->remediation_string = imv_remediation_string_create(
									TRUE, *lang_code);	/* TODO get os_type */

	this->remediation_string->add_instruction(this->remediation_string,
									instr_ports_title,
									instr_ports_descr,
									instr_ports_header,
									this->violating_ports);
	*string = this->remediation_string->get_encoding(this->remediation_string);
	*uri = lib->settings->get_str(lib->settings,
				"libimcv.plugins.imv-scanner.remediation_uri", NULL);

	return TRUE;
}

METHOD(imv_state_t, destroy, void,
	private_imv_scanner_state_t *this)
{
	DESTROY_IF(this->reason_string);
	DESTROY_IF(this->remediation_string);
	this->violating_ports->destroy_function(this->violating_ports, free);
	free(this->ar_id_value.ptr);
	free(this);
}

METHOD(imv_scanner_state_t, add_violating_port, void,
	private_imv_scanner_state_t *this, char *port)
{
	this->violating_ports->insert_last(this->violating_ports, port);
}

/**
 * Described in header.
 */
imv_state_t *imv_scanner_state_create(TNC_ConnectionID connection_id)
{
	private_imv_scanner_state_t *this;

	INIT(this,
		.public = {
			.interface = {
				.get_connection_id = _get_connection_id,
				.has_long = _has_long,
				.has_excl = _has_excl,
				.set_flags = _set_flags,
				.set_max_msg_len = _set_max_msg_len,
				.get_max_msg_len = _get_max_msg_len,
				.set_ar_id = _set_ar_id,
				.get_ar_id = _get_ar_id,
				.change_state = _change_state,
				.get_recommendation = _get_recommendation,
				.set_recommendation = _set_recommendation,
				.get_reason_string = _get_reason_string,
				.get_remediation_instructions = _get_remediation_instructions,
				.destroy = _destroy,
			},
			.add_violating_port = _add_violating_port,
		},
		.state = TNC_CONNECTION_STATE_CREATE,
		.rec = TNC_IMV_ACTION_RECOMMENDATION_NO_RECOMMENDATION,
		.eval = TNC_IMV_EVALUATION_RESULT_DONT_KNOW,
		.connection_id = connection_id,
		.violating_ports = linked_list_create(),
	);

	return &this->public.interface;
}


