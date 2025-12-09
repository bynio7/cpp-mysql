#include <iostream>
#include <fstream>
#include <streambuf>
#include <memory>

#include <yaml-cpp/yaml.h>

#include <mysqlx/xdevapi.h> // MySQL Connector/C++ 8 (X DevAPI)

int main() {
	try {
		// -----------------------
		// 1) Wczytanie config.yaml
		// -----------------------
		YAML::Node cfg = YAML::LoadFile("config.yaml");

		std::string host   = cfg["db"]["host"].as<std::string>();
		std::string user   = cfg["db"]["user"].as<std::string>();
		std::string pass   = cfg["db"]["pass"].as<std::string>();
		std::string schema = cfg["db"]["schema"].as<std::string>();

		std::string query_name = cfg["query"].as<std::string>();
		int limit = cfg["limit"].as<int>();

		// -----------------------
		// 2) Wczytanie pliku SQL
		// -----------------------
		std::ifstream t("queries/" + query_name + ".sql");
		if (!t) {
			std::cerr << "Nie mogę otworzyć pliku SQL.\n";
			return 1;
		}

		std::string sql((std::istreambuf_iterator<char>(t)),
		                 std::istreambuf_iterator<char>());

		// -----------------------
		// 3) Połączenie z MySQL
		// -----------------------
		mysqlx::Session sess(host, 33060, user, pass); 
		sess.sql("USE " + schema).execute();

		// -----------------------
		// 4) Przygotowanie zapytania
		// -----------------------
		auto result = sess.sql(sql).bind(limit).execute();

		// -----------------------
		// 5) Wyświetlenie wyników
		// -----------------------
		for (auto row : result) {
			std::cout << row[0].get<uint64_t>() << " | "
			          << row[1].get<std::string>() << " | "
			          << row[2].get<std::string>() << "\n";
		}
	}
	catch (const std::exception &ex) {
		std::cerr << "Błąd: " << ex.what() << "\n";
	}

	return 0;
}
