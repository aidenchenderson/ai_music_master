#ifndef LLM_HPP
#define LLM_HPP

#include <string>

namespace LLM {


enum class Status {
  Ok,
  Error
};
struct Result {
  Status status = Status::Ok;
  std::string text;
  std::string error;                      
};
struct Request {
  std::string baseUrl = "http://127.0.0.1:11434";//ollama base url
  std::string prompt;//user input
  std::string llmModel = "gemma3:1b";//model name
  
};
Result Generate(const Request& req);


} 
#endif
