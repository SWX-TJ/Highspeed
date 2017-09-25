#include "imageprocthread.h"
#include <QDebug>
ImageProcThread::ImageProcThread()
{

}

void ImageProcThread::run()
{

//    Py_Initialize();
//        if ( !Py_IsInitialized() )
//        {
//            qDebug()<<"init error"<<endl;
//        //return -1;
//        }
//        //导入test.py模块
//        PyObject* pModule = PyImport_ImportModule("hell");
//        if (!pModule) {
//            qDebug()<<"can not open"<<endl;
//              //  printf("Cant open python file!\n");
//               // return -1;
//            }
//        PyObject* pFunhello= PyObject_GetAttrString(pModule,"hello");
//        if(!pFunhello){
//         qDebug()<<"can not get function"<<endl;
//       }
//        //调用hello函数
//  PyObject*  pyreturn = PyObject_CallFunction(pFunhello,NULL);

//  //qDebug()<<ok<<endl;
//        //结束，释放python
//        Py_Finalize();

}
