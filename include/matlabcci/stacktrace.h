#ifndef MATLABCC_STACKTRACE_H_
#define MATLABCC_STACKTRACE_H_

namespace stck {

std::string getstackframe(char *frame, size_t &offset) {
  std::string fr(frame);
  size_t loc0 = fr.find("+");
  size_t loc1 = fr.find("(");
  size_t loc2 = fr.find(")");
  std::stringstream ss;
  
  fr.substr(loc1 + 1, loc2 - loc1 - 1) >> offset;
  std::cout << "AA" << offset << "\n";
  ss << "addr2line -e " << fr.substr(0, loc0) << " -pfC " << fr.substr(loc1 + 1, loc2 - loc1 - 1);
  
  FILE* pipe = popen(ss.str().c_str(), "r");
  char buffer[128];
  
  std::stringstream result;
  while(!feof(pipe))
    if (fgets(buffer, 128, pipe) != NULL)
      result << buffer;
  
  pclose(pipe);
  return result.str();
}

class stacktrace_t {};

stacktrace_t stacktrace;

std::ostream &operator<<(std::ostream &out, stacktrace_t) {
  out << "stacktrace:\n";
  
  size_t max = 256;
  void **stackframes = new void *[max];
  size_t numel;
  while ((numel = backtrace(stackframes, max)) >= max) {
    max *= 2;
    delete[] stackframes;
    stackframes = new void *[max];
  }
  
  char **symbols = backtrace_symbols(stackframes, numel);
  for(size_t i = 0; i < numel; ++i)
    out << getstackframe(symbols[i]);
  
  out << '\n';
  
  for(size_t i = 0; i < numel; ++i)
    out << symbols[i] << '\n';
  
  out << '\n';
  
  delete[] stackframes;
  return out;
}


}

#endif // MATLABCC_STACKTRACE_H_