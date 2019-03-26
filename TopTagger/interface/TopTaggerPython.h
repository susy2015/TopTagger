#ifndef TOPTAGGERPYTHON_H
#define TOPTAGGERPYTHON_H

#ifdef DOPYCAPIBIND
#include "Python.h"
#include "TPython.h"
#include "numpy/arrayobject.h"
#endif

#include "TTreeReaderArray.h"
#include <vector>
#include <memory>

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

        void translatePyList(PyObject* pList)
        {
            len_ = PyList_Size(pList);

            //unfortunately we are going to have to copy the data to make it contiguous
            tmpVec_.reset(new std::vector<T>(len_));

            for(Py_ssize_t i = 0; i < len_; ++i)
            {
                PyObject* item = PyList_GetItem(pList, i);

                if(PyFloat_Check(item))
                {
                    (*tmpVec_)[i] = static_cast<T>(PyFloat_AsDouble(item));
                    buf_ = tmpVec_->data();
                }
                else if(PyInt_Check(item))
                {
                    (*tmpVec_)[i] = static_cast<T>(PyInt_AsLong(item));
                    buf_ = tmpVec_->data();
                }
                else
                {
                    len_ = 0;
                    buf_ = nullptr;
                }
            }
        }

        void setVecBuffer(std::vector<T>& vec)
        {
            len_ = vec.size();
            buf_ = vec.data();
        }
#else
        T dummy;
#endif
        unsigned int len_;
        T* buf_;
        std::unique_ptr<std::vector<T>> tmpVec_;
        
    public:
#ifdef DOPYCAPIBIND
        Py_buffer_wrapper(PyObject* buf, int len = 0) : pObj_(buf), tmpVec_(nullptr)
        {
            if(!pObj_ || (pObj_ == Py_None)) //Check if this is a invalid pointer
            {
                len_ = 0;
                buf_ = nullptr;
            }
            else if(PyArray_Check(pObj_))  //Check if this is a python numpy aray
            {
                PyArrayObject* array = PyArray_GETCONTIGUOUS(reinterpret_cast<PyArrayObject*>(pObj_));
                if(PyArray_NDIM(array) == 1)
                {
                    len_ = PyArray_Size(pObj_);
                    buf_ = reinterpret_cast<T*>(PyArray_DATA(array));
                }
                else
                {
                    //invalid numpy array
                    len_ = 0;
                    buf_ = nullptr;
                }
            }
            else if(PyList_Check(pObj_))  //Check if this is a python list
            {
                translatePyList(pObj_);
            }
            else if(TPython::ObjectProxy_Check(pObj_))  //Check if this is a root generated object 
            {
                //Sloppy type checking ... but its better than no type checking
                if(strstr(Py_TYPE(pObj_)->tp_name, "TTreeReaderArray"))
                {
                    //(unsafe) cast pointer as TTreeReader, can we do more type checking?
                    TTreeReaderArray<T> * tra = static_cast<TTreeReaderArray<T>*>(TPython::ObjectProxy_AsVoidPtr(pObj_));

                    len_ = tra->GetSize();
                    buf_ = static_cast<T*>(tra->GetAddress());
                }
                else if(strstr(Py_TYPE(pObj_)->tp_name, "vector"))
                {
                    //(unsafe) cast pointer as TTreeReader, can we do more type checking?
                    std::vector<T>* vec = static_cast<std::vector<T> *>(TPython::ObjectProxy_AsVoidPtr(pObj_));
            
                    setVecBuffer(*vec);
                }
                else
                {
                    //invalid object
                    len_ = 0;
                    buf_ = nullptr;
                }
            }
            else  //This must be a basic T* pointer
            {
                len_ = len;
                buf_ = reinterpret_cast<T*>(buffer_get(pObj_));
            }
        }
#else
        Py_buffer_wrapper(void* buf) : len_(0), buf_(&dummy) {}
#endif

        //remove copy constructor
        Py_buffer_wrapper(Py_buffer_wrapper&) = delete;
        
        //move constructor 
        Py_buffer_wrapper(Py_buffer_wrapper&& old) : pObj_(old.pObj_), len_(old.len_), buf_(old.buf_), tmpVec_(old.tmpVec_.release()) { }

        //we must subsume!
        Py_buffer_wrapper(std::vector<T>&& buf) : pObj_(nullptr), tmpVec_(new std::vector<T>(std::move(buf)))
        {
            setVecBuffer(*tmpVec_);
        }

        ~Py_buffer_wrapper() {}

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

#ifdef DOPYCAPIBIND
    template<>
    void Py_buffer_wrapper<TLorentzVector>::translatePyList(PyObject* pList)
    {
        (void)pList;
        len_ = 0;
        buf_ = nullptr;
    }

    template<>
    void Py_buffer_wrapper<bool>::translatePyList(PyObject* pList)
    {
        len_ = PyList_Size(pList);
        buf_ = new bool[len_];

        //unfortunately we are going to have to copy the data to make it contiguous                                                                                                                                                       
        for(Py_ssize_t i = 0; i < len_; ++i)
        {
            PyObject* item = PyList_GetItem(pList, i);

            if(PyBool_Check(item))
            {
                buf_[i] = static_cast<bool>(PyInt_AsLong(item));
            }
            else
            {
                delete [] buf_;
                len_ = 0;
                buf_ = nullptr;
            }
        }
    }


    template<>
    void Py_buffer_wrapper<bool>::setVecBuffer(std::vector<bool>& vec)
    {
        len_ = vec.size();
        buf_ = new bool[len_];
        
        for(unsigned int i = 0; i < len_; ++i) buf_[i] = vec[i];
    }

    template<>
    Py_buffer_wrapper<bool>::~Py_buffer_wrapper() 
    {
        if(buf_) delete [] buf_;
    }
#endif

}

#endif
