#ifndef TOPTAGGERPYTHON_H
#define TOPTAGGERPYTHON_H

#ifdef DOPYCAPIBIND
#include "Python.h"
#include "TPython.h"
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
        unsigned int buffer_length( PyObject* self )
        {
            // Retrieve the (type-strided) size of the the buffer; may be a guess.                                                                                                                                                                    
#if PY_VERSION_HEX < 0x03000000
            Py_ssize_t nlen = (*(PyBuffer_Type.tp_as_sequence->sq_length))(self);
#else
            Py_buffer bufinfo;
            (*(Py_TYPE(self)->tp_as_buffer->bf_getbuffer))( self, &bufinfo, PyBUF_SIMPLE );
            Py_ssize_t nlen = bufinfo.len;
#endif
            if ( nlen != INT_MAX )  // INT_MAX is the default, i.e. unknown actual length                                                                                                                                                             
                return nlen;

            return (unsigned int)(nlen);            // return nlen after all, since have nothing better                                                                                                                                               
        }

        ///This function is copied from the ROOT source as it is not exposed to the user ... ... ... thanks ROOT
        const char* buffer_get( PyObject* self)
        {

#if PY_VERSION_HEX < 0x02050000
            const char* buf = 0;
#else
            char* buf = 0;     // interface change in 2.5, no other way to handle it                                                                                                                                                                  
#endif
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
        Py_buffer_wrapper(PyObject* buf) : pObj_(buf)
        {
            if(pObj_)
            {
                len_ = buffer_length(pObj_);
                buf_ = (T*)(buffer_get(pObj_));
            }
            else
            {
                len_ = 0;
                buf_ = nullptr;
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

    std::vector<Constituent> packageConstituentsAK4(PyObject* jet_pt, PyObject* jet_eta, PyObject* jet_phi, PyObject* jet_mass, PyObject* jet_btag, PyDictObject* extraVars);

}

#endif
