#ifndef MYSQLXX_QUERY_H
#define MYSQLXX_QUERY_H

#include <ostream>

#include <mysqlxx/UseQueryResult.h>
#include <mysqlxx/StoreQueryResult.h>


namespace mysqlxx
{


/** Запрос.
  * Ссылается на Connection. Если уничтожить Connection, то Query станет некорректным и пользоваться им будет нельзя.
  *
  * Пример использования:
  *		mysqlxx::Query query = connection.query();
  *		query << "SELECT 1 AS x, 2 AS y, 3 AS z";
  *		query << " LIMIT 1";
  *		mysqlxx::UseQueryResult result = query.use();
  *
  *		while (mysqlxx::Row row = result.fetch())
  *			std::cout << row["x"] << std::endl;
  *
  * В отличие от библиотеки mysql++, запрос можно копировать.
  * (то есть, запрос можно класть в контейнеры STL и ничего с ним не будет)
  */
class Query : public std::ostream
{
public:
	Query(Connection * conn_, const std::string & query_string = "");
	Query(const Query & other);
	Query & operator= (const Query & other);

	/** Сбросить текст запроса. Это используется, если нужно написать новый запрос в том же объекте. */
	void reset();

	/** Выполнить запрос, результат которого не имеет значения (почти всё кроме SELECT). */
	void execute();

	/** Выполнить запрос, с возможностью загружать на клиента строки одна за другой.
	  * То есть, оперативка расходуется только на одну строку.
	  */
	UseQueryResult use();

	/** Выполнить запрос с загрузкой на клиента всех строк.
	  * Требуется оперативка, чтобы вместить весь результат, зато к строкам можно обращаться в произвольном порядке.
	  */
	StoreQueryResult store();

	/// Значение auto increment после последнего INSERT-а.
	UInt64 insertID();

	/// Для совместимости, то же, что insertID().
	UInt64 insert_id() { return insertID(); }

	/// Получить текст запроса (например, для вывода его в лог). См. ещё operator<< ниже.
	std::string str() const
	{
		return query_buf.str();
	}

private:
	Connection * conn;
	std::stringbuf query_buf;

	void executeImpl();
};


/// Вывести текст запроса в ostream.
inline std::ostream & operator<< (std::ostream & ostr, const Query & query)
{
	return ostr << query.rdbuf();
}


}

#endif
