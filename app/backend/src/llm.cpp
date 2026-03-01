#include "llm.hpp"
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <string>

namespace LLM {

static size_t WriteCallback(void* content, size_t size, size_t n, void* ptr) {
  size_t total = size * n;
  auto* s = static_cast<std::string*>(ptr);
  s->append(static_cast<char*>(content), total);
  return total;
}

static Result CallLLM(const Request& request){
  Result result;

  nlohmann::json body = {
    {"model", request.llmModel},
    {"prompt", request.prompt},
    {"stream", false}
  };

  std::string responseStr;

  CURL* curl = curl_easy_init();
  if (!curl) {
    result.status = Status::Error;
    result.error = "libcurl initialization failed!";
    return result;
  }

  std::string url = request.baseUrl + "/api/generate";
  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  std::string bodyStr = body.dump();
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());

  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseStr);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  //checking ollama json response
  if (res != CURLE_OK) {
    result.status = Status::Error;
    result.error = std::string("Ollamao response error is:") + curl_easy_strerror(res);
    result.text = responseStr; 
    return result;
  }
  
  try {
    auto json=nlohmann::json::parse(responseStr);
    if (json.contains("response") &&json["response"].is_string()){
      result.status = Status::Ok;
      result.text=json["response"].get<std::string>();
      return result;
    }

    result.status = Status::Error;
    result.error = "error in Ollama response";
    result.text = responseStr;
    return result;

  } 
  catch (const std::exception& error){
    result.text = responseStr;
    result.status = Status::Error;
    result.error = std::string("JSON parse error is: ") + error.what();
    return result;
  }
}

Result Generate(const Request& request){
  return CallLLM(request);
}
}//namespace LLM

