# ENG
# High-level integration of C++ with MySQL

We use Connector/C++, the library required for high-level database connections.

[connector/c++](https://dev.mysql.com/downloads/connector/cpp/)

Unpack the files from the tar archive into /opt/mysql-connector or adjust the linking entries in CMakeLists.txt accordingly.

Other dependencies:
~~~
sudo apt install g++ build-essential cmake git libssl-dev libboost-all-dev libyaml-cpp-dev
~~~    

## Build:
    git clone https://github.com/bynio7/cpp-mysql
    mkdir cpp-mysql/build
    cd cpp-mysql/build
    cmake ..
    build -j$(nproc)
    mv kod ../
    cd ..

## Usage
Modify the config.yaml file and the sql files from the queries directory according to your needs and run the program:
~~~
./kod
~~~


# PL
# Integracja C++ z MySQL wysokopoziomowo

Używamy Connector/C++ czyli biblioteki wymaganej do wysokopoziomowego łączenia z bazą danych

[connector/c++](https://dev.mysql.com/downloads/connector/cpp/)

należy rozpakować pliki z archiwum tar w /opt/mysql-connector lub odpowiednio dostosować wpisy linkujące w CMakeLists.txt

Inne zależności:
~~~
sudo apt install g++ build-essential cmake git libssl-dev libboost-all-dev libyaml-cpp-dev
~~~	

## Build:
	git clone https://github.com/bynio7/cpp-mysql
	mkdir cpp-mysql/build
	cd cpp-mysql/build
	cmake ..
	build -j$(nproc)
	mv kod ../
	cd ..

## Użycie
Zmieniamy plik config.yaml i pliki sql z katalogu queries według potrzeb i wywołujemy program:
~~~
./kod
~~~	
