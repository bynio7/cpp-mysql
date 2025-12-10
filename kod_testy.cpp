#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iomanip>
#include <memory>
#include <openssl/evp.h>

#include <yaml-cpp/yaml.h>

#include <mysqlx/xdevapi.h> // MySQL Connector/C++

using namespace std;

int main() {
	try {
		// Wczytanie config.yaml
		YAML::Node cfg = YAML::LoadFile("config.yaml");

		string host   = cfg["db"]["host"].as<string>();
		string user   = cfg["db"]["user"].as<string>();
		string pass   = cfg["db"]["pass"].as<string>();
		string schema = cfg["db"]["schema"].as<string>();

		string query_name = cfg["query"].as<string>();
		int limit = cfg["limit"].as<int>();

		// Wczytanie pliku SQL
		ifstream t("queries/" + query_name + ".sql");
		if (!t) {
			cerr << "Nie mogę otworzyć pliku SQL.\n";
			return 1;
		}

		string sql((istreambuf_iterator<char>(t)),
		                 istreambuf_iterator<char>());

		// Połączenie z MySQL
		mysqlx::Session sess(host, 33060, user, pass);
		sess.sql("USE " + schema).execute();

		// Przygotowanie zapytania
		auto result = sess.sql(sql).bind(limit).execute();

		// Wyświetlenie wyników zapytania sql
		size_t num_col = result.getColumnCount();
		for (auto row : result) {
			for (size_t i = 0; i < num_col; i++) {
				if (i>0) {
					cout << " | ";
				}
				try {
					cout << row[i].get<string>();
				} catch (...) {
					try {
						cout << row[i].get<int64_t>();
					} catch (...) {
						try {
							cout << row[i].get<double>();
						} catch (...) {
							cout << "NULL";
						}
					}
				}
			}
			cout << "\n";
		}
		cout << "\n";
		for (auto row : result) {
			// 1. Tworzymy kontekst dla algorytmu SHA256
			EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
			const EVP_MD* md = EVP_sha256();
			
			// Inicjalizacja
			EVP_DigestInit_ex(mdctx, md, NULL);

			for (size_t i = 0; i < num_col; i++) {
				if (i > 0) cout << " | ";

				// Lambda do "karmienia" algorytmu kawałkami danych
				auto process_value = [&](string s) {
					cout << s;
					EVP_DigestUpdate(mdctx, s.c_str(), s.length());
				};

				try {
					process_value(row[i].get<string>());
				} catch (...) {
					try {
						process_value(to_string(row[i].get<int64_t>()));
					} catch (...) {
						try {
							ostringstream strs;
							strs << row[i].get<double>();
							string val = strs.str();
							cout << val;
							EVP_DigestUpdate(mdctx, val.c_str(), val.length());
						} catch (...) {
							process_value("NULL");
						}
					}
				}
			}

			// 2. Finalizacja (obliczenie skrótu)
			unsigned char hash[EVP_MAX_MD_SIZE];
			unsigned int hash_len;
			EVP_DigestFinal_ex(mdctx, hash, &hash_len);
			
			// 3. Sprzątanie kontekstu (ważne w C++!)
			EVP_MD_CTX_free(mdctx);

			// Wyświetlenie wyniku
			cout << "  [EVP-HASH: ";
			for (unsigned int i = 0; i < 4; i++) printf("%02x", hash[i]);
			cout << "]\n";
		}
	}
	catch (const exception &ex) {
		cerr << "Błąd: " << ex.what() << "\n";
	}

	return 0;
}
