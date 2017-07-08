#pragma once

#include <string>
#include <vector>
#include <unordered_map>
//#include <initializer_list>
#include <stack>
#include <cctype>

namespace howtowrite {

	enum class jsonType {
		Decimal = 3,
		Float = 1,
		Boolean = 2,
		Null = 0,
		String = 4,
		Array = 5,
		Object = 6
	};
	
	enum class jsonToken {
		Comma = ',',
		Quote = '"',
		Slash = '/',
		ObjectBegin = '{',
		ObjectEnd = '}',
		ArrayBegin = '[',
		ArrayEnd = ']',
		Colon = ':'
	};
	
	class json final {
	
		class reader final {
			private:
				std::istream& in;
				std::string buffer;
				template<class T> void to(T&);
				jsonType is();
			public:
				reader(std::istream&);
				reader(const reader&) = delete;
				reader& operator=(const reader&) = delete;
				~reader();
				void read(json&);
		};
				
		public:	
			friend std::ostream& operator << (std::ostream&,const json&);
			friend std::istream& operator >> (std::istream&, json&);
			typedef std::vector<json> array;
			typedef std::unordered_map<std::string, json> object;
	
		private:		
				
			void *m_ptr;
			jsonType m_type;
	
			template<class T> jsonType convert() const;
			void copy(const json& j);
			void free();
			void read(json&);
			void write(std::ostream&, int) const;
	
		public:	
			//TODO: slove initializer_list<json> and initializer_list<pair<const char*, json> ambiguous
			template<class T> json(const T &);
			json(array&&);
			json(object&&);
			json(const char*);
			json(const json&);
			json(json&&);
						

			json(); 
			~json();

			template<class T> json& operator=(const T&);
			template<class T> T& as() const;		
			json& operator=(const json&);
			json& operator[] (unsigned int);
			json& operator[] (const std::string&);
			

	};	

}

namespace howtowrite {

//class json
//private
// template methods
	template<> jsonType json::convert<int>() const {
		return jsonType::Decimal;				
	}
	template<> jsonType json::convert<float>() const {
		return jsonType::Float;				
	}
	template<> jsonType json::convert<std::string>() const {
		return jsonType::String;				
	}
	template<> jsonType json::convert<bool>() const {
		return jsonType::Boolean;				
	}
	template<> jsonType json::convert<json::object>() const {
		return jsonType::Object;				
	}
	template<> jsonType json::convert<json::array>() const {
		return jsonType::Array;				
	}
// methods
	void json::free() {
		switch(m_type) {
			case jsonType::Null:
			return;
			case jsonType::Decimal:
				delete static_cast<int*>(m_ptr);
			break;	
			case jsonType::Float:
				delete static_cast<float*>(m_ptr);
			break;	
			case jsonType::Boolean:
				delete static_cast<bool*>(m_ptr);
			break;	
			case jsonType::String:
				delete static_cast<std::string*>(m_ptr);
			break;	
			case jsonType::Object:
				delete static_cast<object*>(m_ptr);
			break;	
			case jsonType::Array:
				delete static_cast<array*>(m_ptr);
			break;	
		}
		m_type = jsonType::Null;
	}
	void json::copy(const json& j) {
		free();
		m_type = j.m_type;
		switch(j.m_type) {
			case jsonType::Null:
			return;
			case jsonType::Decimal:
				m_ptr = new int(*static_cast<int*>(j.m_ptr));		
			return;
			case jsonType::Float:
				m_ptr = new float(*static_cast<float*>(j.m_ptr));		
			return;
			case jsonType::Boolean:
				m_ptr = new bool(*static_cast<bool*>(j.m_ptr));		
			return;
			case jsonType::String: 
				m_ptr = new std::string(*static_cast<std::string*>(j.m_ptr));
			return;
			case jsonType::Object:  
				m_ptr = new object(*static_cast<object*>(j.m_ptr));
			return;
			case jsonType::Array: 
				m_ptr = new array(*static_cast<array*>(j.m_ptr));
			return;		
			
		}
	}
	/*TODO: 
		1. Kill recursion.
		2. Add stream manip
	*/
	void json::write(std::ostream& out, int lvl) const {
		
		switch (m_type) {

		case jsonType::Null:
			out << "null";
		break;

		case jsonType::Array:	 {

			out << (char)jsonToken::ArrayBegin;

			auto arr = as<array>();
			auto beg = arr.begin();

			if (beg != arr.end()) {
				while (true) {
					beg->write(out, lvl);
					
					if (++beg == arr.end()) {
						break;
					} else {
						out << (char)jsonToken::Comma;
					}
				}
			}
			out << (char)jsonToken::ArrayEnd;
		}		
		return;
		case jsonType::Object: {
			auto map = as<object>();
			auto beg = map.begin();
			std::string tabs;
			tabs.insert(0, lvl, '\t');
			out << (char)jsonToken::ObjectBegin<<std::endl << tabs;
			if(beg != map.end()) {
				while (true) {
					out << '\t' << (char)jsonToken::Quote << beg->first << (char)jsonToken::Quote << (char)jsonToken::Colon;
					beg->second.write(out, lvl + 1);

					if (++beg != map.end()) {
						out << (char)jsonToken::Comma;
						out << std::endl << tabs;
					} else {
						break;
					}

				}
			}
			out << std::endl << tabs << (char)jsonToken::ObjectEnd;
		}
		return;
		case jsonType::String:
			out <<(char)jsonToken::Quote<< as<std::string>()<<(char)jsonToken::Quote;
		return;
		case jsonType::Boolean:
			out << (as<bool>() ? "true":"false");
		return;
		case jsonType::Decimal:
			out << as<int>();
		return;
		case jsonType::Float:
			out << as<float>();
		return;
		}
	}
//public	
// constructors
	template<class T> json::json(const T &t) {
		m_type = convert<T>();
		m_ptr = new T(t);
	}

	json::json(const json &j) {
		m_type = jsonType::Null;
		copy(j);
	}
	json::json(array&& arr) {
		array* ptr = new array();
		*ptr =  std::move(arr);
		m_type = jsonType::Array;
		m_ptr = ptr;	
	}
	json::json(object&& obj) {
		object* ptr = new object();
		*ptr = move(obj);
		m_type = jsonType::Object;
		m_ptr = ptr;	
	}


	json::json(json&& j) : m_ptr(j.m_ptr), m_type(j.m_type) {
		j.m_ptr = nullptr;
		j.m_type = jsonType::Null;
	}
		
	json::json(const char* str) {
		m_ptr = new std::string(str);
		m_type = jsonType::String;
	}
	
	json::json() {
		m_type = jsonType::Null;
	}
	json::~json() {
		free();
	}
//operators

	json& json::operator[] (unsigned int n) {
		if (m_type != jsonType::Array) {
			free();
			m_ptr = new array();
			m_type = jsonType::Array;
		}
		array& arr = as<array>();
		if (n == arr.size()) {
			arr.push_back(json());
			return arr.back();
		} else if (n > arr.size()) {
			throw -2;
		} else {
			return arr[n];
		}
	}
	json& json::operator[] (const std::string &key) {
		if(m_type != jsonType::Object) {
			free();
			m_ptr = new object();
			m_type = jsonType::Object;
		}
		return as<object>()[key];
	}
	json& json::operator=(const json& right) {
		copy(right);
		return *this;
	}
	template<class T> json& json::operator=(const T& right) {
		free();
		m_type = convert<T>();
		m_ptr = new T(right);
		return *this;
	}

// methods
	template<class T>  T& json::as() const {
		if(convert<T>() != m_type) throw -1;
		return *static_cast<T*>(m_ptr);		
	}
//class json::reader
//private
//methods
	template<> void json::reader::to<int>(int &number) {		
		number = std::stoi(buffer);
	}
	template<> void json::reader::to<bool>(bool &flag) {
		flag = buffer == "true";
	}
	template<> void json::reader::to<std::string>(std::string &str) {
	
		str = buffer.substr(1, buffer.size() - 2);
	}
	template<> void json::reader::to<float>(float & fnumber) {
		fnumber = std::stod(buffer);
	}
	jsonType json::reader::is() {
	
		if(buffer.front() == '\"' && buffer.back() == buffer.front()) {
			return jsonType::String;
		}	

		short status = 0, pos =0;
		while (buffer.size() > pos && (isdigit(buffer[pos++]) || buffer[pos - 1] == '.' && status++ == 0));
		status =pos == buffer.size() ? status : -1;

		if(status == 1) return jsonType::Float;
		else if(status == 0) return jsonType::Decimal;
		else if(buffer == "null") return jsonType::Null;
		else if(buffer == "true" || buffer == "false") return jsonType::Boolean;
		else throw -3;
	}

//public
//constructor
	json::reader::reader(std::istream& in) :in(in) {
	}
	json::reader::~reader(){
	}
//methods
	void json::reader::read(json& j) {
		char eater = 0;
		std::stack<json*> stack;
		stack.push(&j);
		while(stack.size()) {
			json& j = *stack.top(); stack.pop();
			buffer.clear();			
			in >> eater;
			if (j.m_type == jsonType::Object || eater == (char)jsonToken::ObjectBegin) {
				if (j.m_type == jsonType::Object && eater == (char)jsonToken::ObjectEnd) continue;
				in >> eater;
				if (eater != (char)jsonToken::Quote) throw 0;
				while ((in >> eater) && eater != (char)jsonToken::Quote) buffer += eater;
				if (in.eof()) throw 0;
				in >> eater;
				if (eater != (char)jsonToken::Colon) throw 0;
				stack.push(&j);
				stack.push(&j[buffer]);
				continue; 
			} else if (j.m_type == jsonType::Array || eater == (char)jsonToken::ArrayBegin) {						
				if (j.m_type == jsonType::Array && eater == (char)jsonToken::ArrayEnd) continue;
				else if(j.m_type == jsonType::Array && eater != (char)jsonToken::Comma) throw 0;

				stack.push(&j);
				stack.push(j.m_type == jsonType::Array ? &j[j.as<array>().size()] : &j[0]);
				continue; 
			} else if(eater == (char)jsonToken::Quote) {
				while(in.get(eater) && eater != (char)jsonToken::Quote)
					buffer += eater;
				j = buffer;
			} else {
				buffer += eater;
				
				do {
					while(std::isspace(in.peek())) in.ignore();
					eater = (char)in.peek();
					if (eater != (char)jsonToken::Comma 
				            && eater != (char)jsonToken::ArrayEnd && eater != (char)jsonToken::ObjectEnd)
					{buffer += eater; in.ignore();}
					else break;
				}while(1);			
	
				switch(is()) {
					case jsonType::Null:
					break;
					case jsonType::Decimal:
						j = 0;
						to<int>(j.as<int>());
					break;
					case jsonType::Float:
						j = 0.0f;
						to<float>(j.as<float>());
					break;
					case jsonType::Boolean:
						j = false;
						to<bool>(j.as<bool>());
					break;
				}
			}			
		}
	}
//functions
	std::ostream& operator << (std::ostream& out, const json& j) {
		j.write(out, 0);		
		return out;
	}
	std::istream& operator >> (std::istream& in, json& j) {
		json::reader reader(in);
		reader.read(j);
		return in;
	}
}
