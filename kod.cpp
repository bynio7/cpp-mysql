#include <iostream>
#include <fstream>
#include <streambuf>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <openssl/evp.h>

#include <yaml-cpp/yaml.h>

#include <mysqlx/xdevapi.h>

using namespace std;

int main() {
	try {
		// Konfiguracja
		YAML::Node cfg = YAML::LoadFile("config.yaml");
		string host   = cfg["db"]["host"].as<string>();
		string user   = cfg["db"]["user"].as<string>();
		string pass   = cfg["db"]["pass"].as<string>();
		string schema = cfg["db"]["schema"].as<string>();
		string query_name = cfg["query"].as<string>();
		int limit = cfg["limit"].as<int>();

		// Wczytanie SQL
		ifstream t("queries/" + query_name + ".sql");
		if (!t) {
			cerr << "Nie mogę otworzyć pliku SQL.\n";
			return 1;
		}
		string sql((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

		// DB Connect
		mysqlx::Session sess(host, 33060, user, pass);
		sess.sql("USE " + schema).execute();
		auto result = sess.sql(sql).bind(limit).execute();

		size_t num_col = result.getColumnCount();

		vector<string> computed_hashes;

		cout << "--- WYNIKI ZAPYTANIA SQL ---\n";

		// Główna pętla przetwarzania (Single Pass)
		for (auto row : result) {
			// Inicjalizacja OpenSSL dla wiersza
			EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
			const EVP_MD* md = EVP_sha256();
			EVP_DigestInit_ex(mdctx, md, NULL);

			for (size_t i = 0; i < num_col; i++) {
				if (i > 0) cout << " | ";

				// wyświetla na ekran + karmi funkcję haszującą
				auto process = [&](string s) {
					string display_s = s;
					for (char &c : display_s) {
						if (static_cast<unsigned char>(c) < 32) {
							c = ' ';
						}
					}

					cout << display_s;
					EVP_DigestUpdate(mdctx, s.c_str(), s.length());
				};

				try {
					process(row[i].get<string>());
				} catch (...) {
					try {
						process(to_string(row[i].get<int64_t>()));
					} catch (...) {
						try {
							ostringstream strs;
							strs << row[i].get<double>();
							string val = strs.str();
							cout << val;
							EVP_DigestUpdate(mdctx, val.c_str(), val.length());
						} catch (...) {
							process("NULL");
						}
					}
				}
			}
			cout << "\n";

			// Finalizacja hasza
			unsigned char hash[EVP_MAX_MD_SIZE];
			unsigned int hash_len;
			EVP_DigestFinal_ex(mdctx, hash, &hash_len);
			EVP_MD_CTX_free(mdctx);

			// Konwersja na string HEX i zapisanie do wektora
			stringstream hex_ss;
			hex_ss << hex << setfill('0');
			for (unsigned int i = 0; i < hash_len; i++) {
				hex_ss << setw(2) << (int)hash[i];
			}
			computed_hashes.push_back(hex_ss.str());
		}

		cout << "\n--- OBLICZONE SUMY KONTROLNE (SHA256) ---\n";
		int row_idx = 1;
		for (const string& h : computed_hashes) {
			cout << "Row " << row_idx++ << ": " << h << "\n";
		}
	}
	catch (const exception &ex) {
		cerr << "Błąd: " << ex.what() << "\n";
		return 1;
	}

	return 0;
}
