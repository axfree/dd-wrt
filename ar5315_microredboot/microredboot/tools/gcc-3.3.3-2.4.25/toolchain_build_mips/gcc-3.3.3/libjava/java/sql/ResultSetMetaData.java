/* ResultSetMetaData.java -- Returns information about the ResultSet
   Copyright (C) 1999, 2000, 2002 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package java.sql;

/**
 * This interface provides a mechanism for obtaining information about
 * the columns that are present in a <code>ResultSet</code>.
 * <p>
 * Note that in this class column indexes start at 1, not 0.
 *
 * @author Aaron M. Renn (arenn@urbanophile.com)
 */
public interface ResultSetMetaData 
{
  /**
   * The column does not allow NULL's.
   */
  public static final int columnNoNulls = 0;

  /**
   * The column allows NULL's.
   */
  public static final int columnNullable = 1;

  /**
   * It is unknown whether or not the column allows NULL's.
   */
  public static final int columnNullableUnknown = 2;

  /**
   * This method returns the number of columns in the result set.
   *
   * @return The number of columns in the result set.
   * @exception SQLException If an error occurs.
   */
  public int getColumnCount() throws SQLException;

  /**
   * This method test whether or not the column is an auto-increment column.
   * Auto-increment columns are read-only.
   *
   * @param index The index of the column to test.
   * @return <code>true</code> if the column is auto-increment, <code>false</code>
   *         otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isAutoIncrement(int column) throws SQLException;

  /**
   * This method tests whether or not a column is case sensitive in its values.
   *
   * @param index The index of the column to test.
   * @return <code>true</code> if the column value is case sensitive,
   *         <code>false</code> otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isCaseSensitive(int column) throws SQLException;

  /**
   * This method tests whether not the specified column can be used in 
   * a WHERE clause.
   *
   * @param index The index of the column to test.
   * @return <code>true</code> if the column may be used in a WHERE clause,
   *         <code>false</code> otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isSearchable(int column) throws SQLException;

  /**
   * This method tests whether or not the column stores a monetary value.
   *
   * @param index The index of the column to test.
   * @return <code>true</code> if the column contains a monetary value,
   *         <code>false</code> otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isCurrency(int column) throws SQLException;

  /**
   * This method returns a value indicating whether or not the specified
   * column may contain a NULL value.
   *
   * @param index The index of the column to test.
   * @return A constant indicating whether or not the column can contain NULL,
   *         which will be one of <code>columnNoNulls</code>,
   *         <code>columnNullable</code>, or <code>columnNullableUnknown</code>.
   * @exception SQLException If an error occurs.
   */
  public int isNullable(int column) throws SQLException;

  /**
   * This method tests whether or not the value of the specified column
   * is signed or unsigned.
   *
   * @param index The index of the column to test.
   * @return <code>true</code> if the column value is signed, <code>false</code>
   *         otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isSigned(int column) throws SQLException;

  /**
   * This method returns the maximum number of characters that can be used
   * to display a value in this column.
   *
   * @param index The index of the column to check.
   * @return The maximum number of characters that can be used to display a
   *         value for this column.
   * @exception SQLException If an error occurs.
   */
  public int getColumnDisplaySize(int column) throws SQLException;

  /**
   * This method returns a string that should be used as a caption for this
   * column for user display purposes.
   *
   * @param index The index of the column to check.
   * @return A display string for the column.
   * @exception SQLException If an error occurs.
   */
  public String getColumnLabel(int column) throws SQLException;

  /**
   * This method returns the name of the specified column.
   *
   * @param index The index of the column to return the name of.
   * @return The name of the column.
   * @exception SQLException If an error occurs.
   */
  public String getColumnName(int column) throws SQLException;

  /**
   * This method returns the name of the schema that contains the specified
   * column.
   *
   * @param index The index of the column to check the schema name for.
   * @return The name of the schema that contains the column.
   * @exception SQLException If an error occurs.
   */
  public String getSchemaName(int column) throws SQLException;

  /**
   * This method returns the precision of the specified column, which is the
   * number of decimal digits it contains.
   *
   * @param index The index of the column to check the precision on.
   * @return The precision of the specified column.
   * @exception SQLException If an error occurs.
   */
  public int getPrecision(int column) throws SQLException;

  /**
   * This method returns the scale of the specified column, which is the
   * number of digits to the right of the decimal point.
   *
   * @param index The index column to check the scale of.
   * @return The scale of the column.
   * @exception SQLException If an error occurs.
   */
  public int getScale(int column) throws SQLException;

  /**
   * This method returns the name of the table containing the specified
   * column.
   *
   * @param index The index of the column to check the table name for.
   * @return The name of the table containing the column.
   * @exception SQLException If an error occurs.
   */
  public String getTableName(int column) throws SQLException;

  /**
   * This method returns the name of the catalog containing the specified
   * column.
   *
   * @param index The index of the column to check the catalog name for.
   * @return The name of the catalog containing the column.
   * @exception SQLException If an error occurs.
   */
  public String getCatalogName(int column) throws SQLException;

  /**
   * This method returns the SQL type of the specified column.  This will
   * be one of the constants from <code>Types</code>.
   *
   * @param index The index of the column to check the SQL type of.
   * @return The SQL type for this column.
   * @exception SQLException If an error occurs.
   * @see Types
   */
  public int getColumnType(int column) throws SQLException;

  /**
   * This method returns the name of the SQL type for this column.
   *
   * @param index The index of the column to check the SQL type name for.
   * @return The name of the SQL type for this column.
   * @exception SQLException If an error occurs.
   */
  public String getColumnTypeName(int column) throws SQLException;

  /**
   * This method tests whether or not the specified column is read only.
   *
   * @param index The index of the column to check.
   * @return <code>true</code> if the column is read only, <code>false</code>
   *         otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isReadOnly(int column) throws SQLException;

  /**
   * This method tests whether or not the column may be writable.  This
   * does not guarantee that a write will be successful.
   *
   * @param index The index of the column to check for writability.
   * @return <code>true</code> if the column may be writable,
   *         <code>false</code> otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isWritable(int column) throws SQLException;

  /**
   * This method tests whether or not the column is writable.  This
   * does guarantee that a write will be successful.
   *
   * @param index The index of the column to check for writability.
   * @return <code>true</code> if the column is writable,
   *         <code>false</code> otherwise.
   * @exception SQLException If an error occurs.
   */
  public boolean isDefinitelyWritable(int column) throws SQLException;

  /**
   * This method returns the name of the Java class which will be used to
   * create objects representing the data in this column.
   *
   * @param index The index of the column to check.
   * @return The name of the Java class that will be used for values in
   *         this column.
   * @exception SQLException If an error occurs.
   */
  public String getColumnClassName(int column) throws SQLException;
}
