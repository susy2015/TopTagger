#include "TopTagger/TopTagger/include/hcal/cfg/CfgDocument.hh"
#include <string.h>
#include <stdio.h>
//#include "log4cplus/loggingmacros.h"
//#include "log4cplus/logger.h"
//#include "log4cplus/loglevel.h"
//#include "log4cplus/configurator.h"

/** This is the CfgScript validator.

    Currently, it is only capable of displaying parse errors -- no support for typo searches
*/

bool validate(const std::string& fname);
void help();

int main(int argc, char* argv[]) {
  if (argc<2) {
    help();
    return 0;
  }
  
  //log4cplus::BasicConfigurator config;
  //config.configure();
  
  
  bool ok=true;
  for (int i=1; i<argc; i++) {
    printf("Validating '%s'...\n",argv[i]);
    ok=ok && validate(argv[i]);
  }
  
  return ok?0:1;
}

using namespace hcal::cfg;

bool validate(const std::string& fname) {
  std::string document;
  char buffer[4096];
  FILE* f=fopen(fname.c_str(),"r");
  if (f==0) return false;

  while (!feof(f)) {
    buffer[0]=0;
    char* n=fgets(buffer,4096,f);

    if (n!=0) document+=buffer;
  }
  fclose(f);

  std::auto_ptr<CfgDocument> doc=CfgDocument::parseDocument(document,log4cplus::Logger::getInstance("Validator"));
 
  return true;
}

void help() {
  printf("cfgvalidator [cfg script] [cfg script] ...\n");
}
