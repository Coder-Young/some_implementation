#ifndef PUBLIC_H_
#define PUBLIC_H_

#define LOG(str) \
            std::cout << "file:" << __FILE__ << " line: " << __LINE__ \
            << " time: " << __TIMESTAMP__ << " info: " << str << std::endl;


#endif 