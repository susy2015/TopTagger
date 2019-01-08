#ifndef TOPTAGGERPYTHON_H
#define TOPTAGGERPYTHON_H

#ifdef DOPYCAPIBIND
#include "Python.h"
#include "TPython.h"
#include "TTreeReaderArray.h"
#endif

#include "TopTagger/TopTagger/interface/TopTaggerUtilities.h"

namespace ttPython
{

    /**
     *Wrapper class to take a Py_buffer* and hold it in a vector-like implementation for use in ttUtilities classes.  Overloaded constructor for use with vectors.  In both the case of the PyBuffer and std::vector it is the users responcibility to ensure that the memory pass into this wrapper stays in scope until the wrapper is destroyed .
     */
    template<typename T>
    class Py_buffer_wrapper
    {
    private:
    #ifdef DOPYCAPIBIND
        PyObject* pObj_;

        ///This function is copied from the ROOT source as it is not exposed to the user ... ... ... thanks ROOT
        char* buffer_get( PyObject* self)
        {
            char* buf = 0;
#if PY_VERSION_HEX < 0x03000000
            (*(PyBuffer_Type.tp_as_buffer->bf_getcharbuffer))( self, 0, &buf );
#else
            Py_buffer bufinfo;
            (*(PyBuffer_Type.tp_as_buffer->bf_getbuffer))( self, &bufinfo, PyBUF_SIMPLE );
            buf = (char*)bufinfo.buf;
#endif

            if ( ! buf )
                PyErr_SetString( PyExc_IndexError, "attempt to index a null-buffer" );

            return buf;
        }
#else
        T dummy;
#endif
        unsigned int len_;
        T* buf_;


    public:
#ifdef DOPYCAPIBIND
        Py_buffer_wrapper(PyObject* buf, int len = 0) : pObj_(buf)
        {
            if(!pObj_ || (pObj_ == Py_None)) //Check if this is a invalid pointer
            {
                len_ = 0;
                buf_ = nullptr;
            }
            else if(TPython::ObjectProxy_Check(pObj_))  //Check if this is a root object 
            {
                //it is a root object ... not sure if we can check what type of object because TTreeReader does not derive from TObject
                //(unsafe) cast pointer as TTreeReader
                TTreeReaderArray<T> * tra = static_cast<TTreeReaderArray<T>*>(TPython::ObjectProxy_AsVoidPtr(pObj_));

                len_ = tra->GetSize();
                buf_ = static_cast<T*>(tra->GetAddress());
            }
            else //This must be a basic T* pointer
            {
                len_ = len;
                buf_ = reinterpret_cast<T*>(buffer_get(pObj_));
            }
        }
#else
        Py_buffer_wrapper(void* buf) : len_(0), buf_(&dummy) {}
#endif

        Py_buffer_wrapper(std::vector<T>* buf) : pObj_(nullptr)
        {
            len_ = buf->size();
            buf_ = buf->data();
        }
        
        unsigned int size() const 
        {
            return len_;
        }
    
        T& operator[](unsigned int i)             
        {
            return buf_[i]; 
        }
        const T& operator[](unsigned int i) const 
        {
            return buf_[i]; 
        }
    
        T* begin()             
        {
            return buf_; 
        }
        const T* begin() const 
        {
            return buf_; 
        }
    
        T* end()             
        {
            return buf_ + size(); 
        }
        const T* end() const 
        {
            return buf_ + size(); 
        }    
    };
}

#endif
