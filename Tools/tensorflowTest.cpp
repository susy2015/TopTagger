#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>
#include "/home/pastika/topTagger/tensorflow/tensorflow/c/c_api.h"

TF_Buffer* read_file(const char* file);

void free_buffer(void* data, size_t length) 
{
    free(data);
}


int main() {
    TF_Buffer* graph_def = read_file("frozen.pb");
    TF_Graph* graph = TF_NewGraph();

    // Import graph_def into graph
    TF_Status* status = TF_NewStatus();
    TF_ImportGraphDefOptions* graph_opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_def, graph_opts, status);
    TF_DeleteImportGraphDefOptions(graph_opts);
    if (TF_GetCode(status) != TF_OK) {
        fprintf(stderr, "ERROR: Unable to import graph %s\n", TF_Message(status));
        return 1;
    }
    fprintf(stdout, "Successfully imported graph\n");

    TF_SessionOptions* sess_opts = TF_NewSessionOptions();
    TF_Session* session = TF_NewSession(graph, sess_opts, status);
    TF_DeleteSessionOptions(sess_opts);
    if (TF_GetCode(status) != TF_OK) {
        fprintf(stderr, "ERROR: Unable to import graph %s\n", TF_Message(status));
        return 1;
    }
    fprintf(stdout, "Successfully created session\n");

    const int num_dims = 2;
    const int elemSize = sizeof(float);
    std::vector<int64_t> dims = {2, 16};
    int nelem = 1;
    for(const auto dimLen : dims) nelem += dimLen;
    TF_Tensor* input_values_0 =  TF_AllocateTensor(TF_FLOAT, dims.data(), dims.size(), elemSize*nelem);

    std::vector<float> input_data = {0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,
                                     0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};

    auto data = static_cast<float*>(TF_TensorData(input_values_0));
    //for(int i =  0; i < 32; ++i) data[i] = input_data[i];
    memcpy(data, input_data.data(), input_data.size()*sizeof(float));

    TF_Operation* op_x = TF_GraphOperationByName(graph, "x");
    TF_Operation* op_y = TF_GraphOperationByName(graph, "y");

    printf("%lu, %lu\n", (unsigned long int)(op_x), (unsigned long int)(op_y));

    size_t pos = 0;
    TF_Operation* oper;
    while ((oper = TF_GraphNextOperation(graph, &pos)) != nullptr) 
    {
        printf("Op name: %s\n", TF_OperationName(oper));
    }

    std::vector<TF_Output>     inputs        = {       {op_x, 0} };
    std::vector<TF_Tensor*>    input_values  = {  input_values_0 };

    std::vector<TF_Output>     outputs       = {       {op_y, 0} };
    std::vector<TF_Tensor*>    output_values(1);

    std::vector<TF_Operation*> targets       = { op_y };

    TF_SessionRun(session,
                  // RunOptions
                  nullptr,
                  // Input tensors
                  inputs.data(), input_values.data(), inputs.size(),
                  // Output tensors
                  outputs.data(), output_values.data(), outputs.size(),
                  // Target operations
                  targets.data(), targets.size(),
                  // RunMetadata
                  nullptr,
                  // Output status
                  status);

    if (TF_GetCode(status) != TF_OK) {
        fprintf(stderr, "ERROR: Unable to run graph %s\n", TF_Message(status));
        return 1;
    }
    fprintf(stdout, "Session Run Successfully\n");
    
    auto data_out = static_cast<float*>(TF_TensorData(output_values[0]));
    printf("Output Data: %f, %f, %f, %f\n             %f, %f, %f, %f\n", data_out[0], data_out[1], data_out[2], data_out[3], data_out[4], data_out[5], data_out[6], data_out[7]);

    TF_DeleteTensor(input_values_0);
    for(auto tensor : output_values) TF_DeleteTensor(tensor);
    TF_DeleteSession(session, status);
    TF_DeleteStatus(status);
    TF_DeleteBuffer(graph_def);

    // Use the graph
    TF_DeleteGraph(graph);
    return 0;
}

TF_Buffer* read_file(const char* file) {
    FILE *f = fopen(file, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    void* data = malloc(fsize);
    fread(data, fsize, 1, f);
    fclose(f);

    TF_Buffer* buf = TF_NewBuffer();
    buf->data = data;
    buf->length = fsize;
    buf->data_deallocator = free_buffer;
    return buf;
}

//int main()
//{
//    printf("Hello from TensorFlow C library version %s\n", TF_Version());
//
//
//
//TF_Session* TF_LoadSessionFromSavedModel(
//    const TF_SessionOptions* session_options, const TF_Buffer* run_options,
//    const char* export_dir, const char* const* tags, int tags_len,
//    TF_Graph* graph, TF_Buffer* meta_graph_def, TF_Status* status);
//
//    return 0;
//}

//#include <iostream>
//#include "/home/pastika/topTagger/tensorflow/tensorflow/c/c_api.h"
//
//using namespace std;
//
//int main() {
//    cout << TF_Version();
//    return 0;
//}
