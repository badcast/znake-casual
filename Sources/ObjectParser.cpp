#include "ObjectParser.h"

#include <stdexcept>

#include "ResourceManager.h"

// TOP

namespace jno {

enum { Node_ValueFlag = 1, Node_ArrayFlag = 2, Node_StructFlag = 3 };

struct jno_evaluted {
    jnumber jstrings;
    jnumber jnumbers;
    jnumber jreals;
    jnumber jbools;
};

template <typename T>
[[deprecated]]T* jalloc() {
    return RoninEngine::Runtime::GC::gc_alloc<T>();
}

template <typename T>
[[deprecated]]T* jalloc(T* copy) {
    return RoninEngine::Runtime::GC::gc_alloc<T>(copy);
}

template <typename T>
[[deprecated]]T* jalloc(const T& copy) {
    return RoninEngine::Runtime::GC::gc_alloc<T>(copy);
}

template <typename T>
[[deprecated]]void jfree(T* t) {
    RoninEngine::Runtime::GC::gc_unalloc(t);
}

void jfree(void* t) { RoninEngine::Runtime::GC::gc_free(t); }

void jno_evaluate(jno_evaluted* file) {}

jbool jno_is_jnumber(const char c) { return c >= '0' && c <= '9' || c == '-'; }

jbool jno_is_jreal(const char* c, int* getLength) {
    *getLength = 0;
    bool real = false;
    while (true) {
        if (!jno_is_jnumber(c[*getLength])) {
            if (real) break;

            real = (c[*getLength] == jno_syntax.jno_dot);

            if (!real) break;
        }
        ++*getLength;
    }
    return real;
}

jbool jno_is_jbool(const char* content, int* getLength) {
    if (!memcmp(content, jno_syntax.jno_true_string, *getLength = sizeof(jno_syntax.jno_true_string))) return true;
    if (!memcmp(content, jno_syntax.jno_false_string, *getLength = sizeof(jno_syntax.jno_false_string))) return true;
    *getLength = 0;
    return false;
}

int jno_format(const char* content, void** mem, JNOType& out) {
    int temp = 0;

    if (mem) *mem = nullptr;

    // string type
    if (*content == jno_syntax.jno_format_string) {
        ++temp;
        for (; content[temp] != jno_syntax.jno_format_string; ++temp)
            if (content[temp] == jno_syntax.jno_left_seperator && content[temp + 1] == jno_syntax.jno_format_string) ++temp;
        --temp;
        if (temp) {
            if (mem) {
                jstring str = jstring();
                str.reserve(temp);
                for (size_t i = 0; i < temp; ++i) {
                    if (content[i + 1] == jno_syntax.jno_left_seperator) ++i;
                    str.push_back(content[i + 1]);
                }
                *mem = reinterpret_cast<void*>(jalloc(jstring(str)));
            }
            out = JNOType::JNOString;
        } else
            out = JNOType::Unknown;

        temp += 2;
    } else if (jno_is_jreal(content, &temp)) {
        if (mem) *mem = jalloc<jreal>(static_cast<double>(atof(content)));
        out = JNOType::JNOReal;
    } else if (jno_is_jnumber(*content)) {
        if (mem) *mem = jalloc<jnumber>(atoll(content));
        out = JNOType::JNONumber;
    } else if (config::is_bool(content, &temp)) {
        if (mem) *mem = jalloc<jbool>(temp == sizeof(jno_syntax.jno_true_string) - 1);
        out = JNOType::JNOBoolean;
    } else
        temp = 0;

    return temp;
}

jbool jno_has_datatype(const char* currentPointer) {
    JNOType type;
    jno_format(const_cast<char*>(currentPointer), nullptr, type);
    return type != JNOType::Unknown;
}

int jno_trim(const char* content, int contentLength = INT_MAX) {
    int i = 0, j;
    if (content != nullptr)
        for (; i < contentLength && content[i];) {
            for (j = 0; j < static_cast<int>(sizeof(jno_syntax.jno_trim_segments));)
                if (content[i] == jno_syntax.jno_trim_segments[j])
                    break;
                else
                    ++j;
            if (j != sizeof(jno_syntax.jno_trim_segments))
                ++i;
            else
                break;
        }
    return i;
}

int jno_skip(const char* c, int len = INT_MAX) {
    int i, j;
    char skipping[sizeof(jno_syntax.jno_trim_segments) + sizeof(jno_syntax.jno_array_segments)];
    skipping[0] = '\0';
    strncpy(skipping, jno_syntax.jno_trim_segments, sizeof(jno_syntax.jno_trim_segments));
    strncpy(skipping + sizeof jno_syntax.jno_trim_segments, jno_syntax.jno_array_segments,
            sizeof(jno_syntax.jno_array_segments));
    for (i = 0; c[i] && i <= len;) {
        for (j = 0; j < sizeof(skipping);)
            if (c[i] == skipping[j])
                break;
            else
                ++j;
        if (j == sizeof(skipping))
            ++i;
        else
            break;
    }
    return i;
}

jbool jno_is_property(const char* c, int len) {
    for (size_t i = 0; i < len; ++i)
        if (!(toupper(c[i]) >= 'A' && toupper(c[i]) <= 'Z' || (i > 0 && config::is_number(c[i])) || c[i] == '_')) return false;
    return len != 0;
}

jbool jno_is_comment_line(const char* c, int len) {
    return len > 0 && !strncmp(c, jno_syntax.jno_commentLine, std::min(2, len));
}

int jno_skip_comment(const char* c, int len) {
    int i = 0;
    i += jno_trim(c + i, len - i);
    while (jno_is_comment_line(c + i, len - i)) {
        i += config::go_end_line(c + i, len - i);
        i += jno_trim(c + i, len - i);
    }
    return i;
}

jbool jno_is_space(char c) {
    for (int i = 0; i <= sizeof(jno_syntax.jno_trim_segments); ++i)
        if (jno_syntax.jno_trim_segments[i] == c) return true;
    return false;
}

jbool jno_is_array(const char* content, int& endpoint, int contentLength = INT_MAX) {
    int i = 0;
    jbool result = false;

    if (*content == jno_syntax.jno_array_segments[0]) {
        ++i;
        i += jno_trim(content + i);

        i += jno_skip_comment(content + i, contentLength - i);
        i += jno_trim(content + i);
        if (config::Has_DataType(content + i) || content[i] == jno_syntax.jno_array_segments[1]) {
            for (; content[i] && i < contentLength; ++i) {
                if (content[i] == jno_syntax.jno_array_segments[0])
                    break;
                else if (content[i] == jno_syntax.jno_array_segments[1]) {
                    endpoint = i;
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}

int jno_string_to_hash(const char* content, int contentLength) {
    int x = 1;
    int y = 0;
    while (*(content + y) && y < contentLength) x *= (int)*(content + y++);
    return x;
}

jno_object_node::jno_object_node(const jno_object_node& copy) {
    this->_bits = copy._bits;
    this->valueFlag = copy.valueFlag;
    this->propertyName = copy.propertyName;
    decrementMemory();
    uses = copy.uses;
    incrementMemory();
    copy.uses = uses;
}
jno_object_node& jno_object_node::operator=(const jno_object_node& copy) {
    this->_bits = copy._bits;
    this->valueFlag = copy.valueFlag;
    this->propertyName = copy.propertyName;
    decrementMemory();
    uses = copy.uses;
    incrementMemory();
    copy.uses = uses;
    ++*copy.uses;
    return *this;
}
jno_object_node::~jno_object_node() {
    if (!decrementMemory() && _bits) {
        if (isStruct()) {
            jfree((jno_object_parser::jstruct*)_bits);
        } else if (isArray()) {
            switch (valueFlag >> 2) {
                case JNOType::JNOBoolean:
                    jfree(toBooleans());
                    break;
                case JNOType::JNOString:
                    jfree(toStrings());
                    break;
                case JNOType::JNONumber:
                    jfree(toNumbers());
                    break;
                case JNOType::JNOReal:
                    jfree(toReals());
                    break;
            }
        } else if (isValue()) {
            if (isString()) {
                jfree((jstring*)_bits);
            } else
                jfree(_bits);
        }
    }
}

int jno_object_node::decrementMemory() {
    int res = 0;
    if (uses)
        if (!(res = --(*uses))) {
            res = *uses;
            jfree(uses);
        }
    uses = nullptr;
    return res;
}

int jno_object_node::incrementMemory() {
    if (!uses) uses = jalloc<int>();
    return ++*uses;
}

jbool jno_object_node::isValue() { return (this->valueFlag & 3) == Node_ValueFlag; }
jbool jno_object_node::isArray() { return (this->valueFlag & 3) == Node_ArrayFlag; }
jbool jno_object_node::isStruct() { return (this->valueFlag & 3) == Node_StructFlag; }
jbool jno_object_node::isNumber() { return ((this->valueFlag & 0x1C) >> 2) == JNOType::JNONumber; }
jbool jno_object_node::isReal() { return ((this->valueFlag & 0x1C) >> 2) == JNOType::JNOReal; }
jbool jno_object_node::isString() { return ((this->valueFlag & 0x1C) >> 2) == JNOType::JNOString; }
jbool jno_object_node::isBoolean() { return ((this->valueFlag & 0x1C) >> 2) == JNOType::JNOBoolean; }

jstring& jno_object_node::getPropertyName() { return this->propertyName; }
void jno_object_node::setMemory(void* memory) { this->_bits = memory; }
jnumber& jno_object_node::toNumber() {
    if (!isNumber()) throw std::bad_cast();

    return *(jnumber*)this->_bits;
}
jreal& jno_object_node::toReal() {
    if (!isReal()) throw std::bad_cast();
    return *(jreal*)this->_bits;
}
jstring& jno_object_node::toString() {
    if (!isString()) throw std::bad_cast();
    return *(jstring*)this->_bits;
}
jbool& jno_object_node::toBoolean() {
    if (!isBoolean()) throw std::bad_cast();
    return *(jbool*)this->_bits;
}
vector<jnumber>* jno_object_node::toNumbers() {
    if (!isArray() && (valueFlag >> 2) != JNOType::JNONumber) throw std::bad_cast();
    return (vector<jnumber>*)_bits;
}
vector<jreal>* jno_object_node::toReals() {
    if (!isArray() && (valueFlag >> 2) != JNOType::JNOReal) throw std::bad_cast();
    return (vector<jreal>*)_bits;
}
vector<jstring>* jno_object_node::toStrings() {
    if (!isArray() && (valueFlag >> 2) != JNOType::JNOString) throw std::bad_cast();
    return (vector<jstring>*)_bits;
}
vector<jbool>* jno_object_node::toBooleans() {
    if (!isArray() && (valueFlag >> 2) != JNOType::JNOBoolean) throw std::bad_cast();
    return (vector<jbool>*)_bits;
}

jno_object_parser::jno_object_parser() {}
jno_object_parser::~jno_object_parser() {}

#ifdef _DEBUG
static ObjectNode* _dbgLastNode;
#endif

// big algorithm, please free me.
int jno_object_parser::avail(jno_object_parser::jstruct& entry, const char* source, int len, int levels) {
    int i, j;

    void* memory = nullptr;
    JNOType arrayType;
    JNOType valueType;
    jno_object_node curNode;

    //Базовый случаи
    if (!len) return 0;

    for (i = 0; i < len && source[i];) {
        // has comment line
        j = i += jno_skip_comment(source + i, len - i);
        if (levels > 0 && source[i] == jno_syntax.jno_array_segments[1]) break;
        i += jno_skip(source + i, len - i);
        // check property name
        if (!jno_is_property(source + j, i - j)) throw std::bad_exception();
        curNode = {};
        curNode.propertyName.append(source + j, static_cast<size_t>(i - j));
        i += jno_trim(source + i, len - i);  // trim string
        // has comment line
        i += jno_skip_comment(source + i, len - i);
        i += jno_trim(source + i, len - i);  // trim string
        // is block or array
        if (source[i] == jno_syntax.jno_array_segments[0]) {
            ++levels;
            if (jno_is_array(source + i, j, len - i)) {
                j += i++;
                arrayType = JNOType::Unknown;
                curNode.valueFlag = Node_ArrayFlag;
                for (; i < j;) {
                    i += jno_skip_comment(source + i, j - i);
                    // next index
                    if (source[i] == jno_syntax.jno_obstacle)
                        ++i;
                    else {
                        i += jno_format(source + i, &memory, valueType);
                        if (valueType != JNOType::Unknown) {
                            if (arrayType == JNOType::Unknown) {
                                arrayType = valueType;

                                switch (arrayType) {
                                    case JNOType::JNOString:
                                        curNode.setMemory((void*)jalloc<std::vector<jstring>>());
                                        break;
                                    case JNOType::JNOBoolean:
                                        curNode.setMemory((void*)jalloc<std::vector<jbool>>());
                                        break;
                                    case JNOType::JNOReal:
                                        curNode.setMemory((void*)jalloc<std::vector<jreal>>());
                                        break;
                                    case JNOType::JNONumber:
                                        curNode.setMemory((void*)jalloc<std::vector<jnumber>>());
                                        break;
                                }

                            } else if (arrayType != valueType && arrayType == JNOType::JNOReal &&
                                       valueType != JNOType::JNONumber) {
                                throw std::runtime_error("Multi type is found.");
                            }

                            switch (arrayType) {
                                case JNOType::JNOString: {
                                    auto ref = (jstring*)memory;
                                    ((vector<string>*)curNode._bits)->emplace_back(*ref);
                                    jfree(ref);
                                    break;
                                }
                                case JNOType::JNOBoolean: {
                                    auto ref = (jbool*)memory;
                                    ((vector<jbool>*)curNode._bits)->emplace_back(*ref);
                                    jfree(ref);
                                    break;
                                }
                                case JNOType::JNOReal: {
                                    jreal* ref;
                                    if (valueType == JNOType::JNONumber) {
                                        ref = new jreal(static_cast<jreal>(*(jnumber*)memory));
                                        jfree((jnumber*)memory);
                                    } else
                                        ref = (jreal*)memory;

                                    ((vector<jreal>*)curNode._bits)->emplace_back(*ref);
                                    jfree(ref);
                                    break;
                                }
                                case JNOType::JNONumber: {
                                    auto ref = (jnumber*)memory;
                                    ((vector<int64_t>*)curNode._bits)->emplace_back(*ref);
                                    jfree(ref);
                                    break;
                                }
                            }
                        }
                    }
                    i += jno_skip_comment(source + i, j - i);
                }

                // shrink to fit
                switch (arrayType) {
                    case JNOType::JNOString: {
                        ((vector<string>*)curNode._bits)->shrink_to_fit();
                        break;
                    }
                    case JNOType::JNOBoolean: {
                        ((vector<jbool>*)curNode._bits)->shrink_to_fit();
                        break;
                    }
                    case JNOType::JNOReal: {
                        ((vector<jreal>*)curNode._bits)->shrink_to_fit();
                        break;
                    }
                    case JNOType::JNONumber: {
                        ((vector<int64_t>*)curNode._bits)->shrink_to_fit();
                        break;
                    }
                }

                curNode.valueFlag |= (arrayType) << 2;
            } else {  // get the next node
                ++i;
                jstruct* _nodes = jalloc<jstruct>();
                i += avail(*_nodes, source + i, len - i, levels);
                curNode.valueFlag = Node_StructFlag;
                curNode.setMemory(_nodes);
                i += jno_skip_comment(source + i, len - i);
            }
            --levels;
            if (source[i] != jno_syntax.jno_array_segments[1]) {
                throw std::bad_exception();
            }
            ++i;
        } else {  // get also value
            i += jno_format(source + i, &memory, valueType);
            curNode.setMemory(memory);
            memory = nullptr;
            curNode.valueFlag = Node_ValueFlag | valueType << 2;
        }

        entry.insert(make_pair(j = jno_string_to_hash(curNode.propertyName.c_str()), curNode));

#if defined(QDEBUG) || defined(DEBUG)
        // get iter
        auto _curIter = entry.find(j);
        if (_dbgLastNode) _dbgLastNode->nextNode = &_curIter->second;
        _curIter->second.prevNode = _dbgLastNode;
        _dbgLastNode = &_curIter->second;
#endif
        i += jno_skip_comment(source + i, len - i);
    }

    return i;
}
void jno_object_parser::deserialize(const char* filename) {
    long sz;
    char* buf;
    std::ifstream file;
    file.open(filename);
    sz = file.seekg(0, std::ios::end).tellg();
    file.seekg(0, std::ios::beg);

    if ((buf = (char*)malloc(sz)) == nullptr) throw std::bad_alloc();

    memset(buf, 0, sz);
    sz = file.read(buf, sz).gcount();
    file.close();
    deserialize((char*)buf, sz);
    free(buf);
}
void jno_object_parser::deserialize(const jstring& source) { deserialize(source.data(), source.size()); }
void jno_object_parser::deserialize(const char* source, int len) {
    int i;

    entry.clear();  // clears alls
#ifdef _DEBUG
    _dbgLastNode = nullptr;
#endif
    i = jno_object_parser::avail(entry, source, len);

    auto node = find_node("first/ops/ps");
}
jstring jno_object_parser::serialize() {
    jstring data;
    throw std::runtime_error("no complete");
    return data;
}
jno_object_node* jno_object_parser::at(const jstring& name) {
    jno_object_node* node = nullptr;
    int id = jno_string_to_hash(name.c_str());

    auto iter = this->entry.find(id);

    if (iter != std::end(this->entry)) node = &iter->second;

    return node;
}

jno_object_parser::jstruct& jno_object_parser::get_struct() { return entry; }

jno_object_node* jno_object_parser::find_node(const jstring& nodePath) {
    jno_object_node* node = nullptr;
    decltype(this->entry)* entry = &this->entry;
    decltype(entry->begin()) iter;
    int l = 0, r;
    int hash;
    // get splits
    do {
        if ((r = nodePath.find(jno_syntax.jno_nodePathBreaker, l)) == ~0) r = static_cast<int>(nodePath.length());
        hash = jno_string_to_hash(nodePath.c_str() + l, r - l);
        iter = entry->find(hash);
        if (iter != end(*entry)) {
            if (iter->second.isStruct())
                entry = decltype(entry)(iter->second._bits);
            else {
                if (r == nodePath.length()) node = &iter->second;  // get the next section
                break;
            }
        }
        l = r + 1;
        r = nodePath.length();

    } while (l < r);
    return node;
}
jbool jno_object_parser::contains(const jstring& nodePath) { return find_node(nodePath) != nullptr; }

}  // namespace jno
