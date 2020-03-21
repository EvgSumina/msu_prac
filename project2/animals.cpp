#include <iostream>
#include <string.h>
using namespace std;

class animal {
	char* name;
	char* sponsor;
public:
	animal(char* n, char* sp) {
		name = new char[strlen(n) + 1];
		strcpy(name, n);
		sponsor = new char[strlen(sp) + 1];
		strcpy(sponsor, sp);
	}

	animal(const animal& other) {
		name = new char[strlen(other.name) + 1];
		strcpy(name, other.name);
		sponsor = new char[strlen(other.sponsor) + 1];
		strcpy(sponsor, other.sponsor);
	}

	char* getname() const {
		return name;
	}

	char* getsponsor() const {
		return sponsor;
	}

	const animal& operator= (const animal& other) {
		if (this == &other) 
			return *this;
		if (name) 
			delete []name;
		name = new char[strlen(other.name) + 1];
		strcpy(name, other.name);
		if (sponsor) 
			delete [] sponsor;
		sponsor = new char[strlen(other.sponsor) + 1];
		strcpy(sponsor, other.sponsor);
		return *this;
	}

	virtual void print(ostream& s) const = 0;

	virtual ~animal() {
		delete [] name;
		delete [] sponsor;
	}
};

class mammal: public animal {
	bool tail;
	bool wool;
public:
	mammal(char* n, char* sp, bool a, bool b): animal(n, sp) {
		tail = a;
		wool = b;
	}

	mammal(const mammal& other): animal(other) {	
		tail = other.tail;
		wool = other.wool;
	}

	const mammal& operator= (const mammal& other) {
		if (this == &other)
			return *this;
		animal::operator=(other);
		tail = other.tail;
		wool = other.wool;
		return *this;
	}

	bool gettail() const {
		return tail;
	}

	bool getwool() const {
		return wool;
	}

	virtual void print(ostream& s) const{
		s << "mammal: " << getname();
		s << " : " << getsponsor() << endl;
		s << "tail: " << tail << ": wool: " << wool << endl;
	}

	virtual ~mammal() {};
};

class bird: public animal {
	char* colour;
public:
	bird(char* n, char* sp, char* cl): animal(n, sp) {
		colour = new char[strlen(cl) + 1];
		strcpy(colour, cl);
	}

	bird(const bird& other): animal(other) {	
		colour = new char[strlen(other.colour) + 1];
		strcpy(colour, other.colour);
	}

	const bird& operator= (const bird& other) {
		if (this == &other) 
			return *this;
		animal::operator=(other);
		if (colour) delete [] colour;
		colour = new char[strlen(other.colour) + 1];
		strcpy(colour, other.colour);
		return *this;
	}

	char* getcolour() const {
		return colour;
	}

	virtual void print(ostream& s) const{
		s << "bird: " << getname();
		s << " : " << getsponsor() << endl;
		s << "colour: " << colour << endl;
	}

	virtual ~bird() {
		delete [] colour;
	}
};

ostream& operator<<(ostream& s, const animal& an) {
	an.print(s);
	return s;
}

int main() {
	char *nam = new char[5];
	strcpy(nam, "lion");
	char *spon = new char[5];
	strcpy(spon, "addy");
	char *nam1 = new char[4];
	strcpy(nam1, "cat");
	char *spon1 = new char[5];
	strcpy(spon1, "John");
	mammal a(nam, spon, 1, 1);
	mammal b(a);
	cout<<a;
	cout<<b;
	return 0;
}
