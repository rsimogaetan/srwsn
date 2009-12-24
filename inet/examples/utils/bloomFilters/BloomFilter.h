#ifndef H_GL_BLOOMFILTER
#define H_GL_BLOOMFILTER

class BloomFilter {
public:
	BloomFilter(const char * name);  // The Constructor
	~BloomFilter();                  // The Destructor
	void toString();				 // For information
private:
	char * my_name;
};

#endif

