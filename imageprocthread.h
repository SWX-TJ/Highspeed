#ifndef IMAGEPROCTHREAD_H
#define IMAGEPROCTHREAD_H
#include <QObject>
#include <QThread>
#include <opencv.hpp>
#include <QFileInfo>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QMessageBox>
#include <QTextStream>
#include <opencv2/imgproc.hpp>
#include <autotemplete.h>
using namespace cv;
using namespace autotemepfunc;
class ImageProcThread : public QThread
{
    Q_OBJECT
public:
    ImageProcThread();
public:
    Mat OriImage;//加载的原始图
    Mat TempImage;//模板图像
    QImage dispTempImage;//模板图像显示
    QImage dispImage;//主显示图像
    Mat resuImage;
    double minValue,maxValue;
    Point minLocation;
    Point maxLocation;
    Point matchLocation;
    std::vector<Mat> tempImages;//模板图片
    std::vector<Rect> MatchRects;
    std::vector<float>line_k;
    QString MatchImageFilePath;
    QList<QFileInfo> *MatchImageFileInfo;
    QList<QFileInfo> *TempleteImageFileInfo;
    int TempleteImageNum;
    int MatchImageNum;
    int minMatchArea;
    float max_k;
    bool isTempleteImage;
    bool isLoadTempleteImage;
    QString SpendTime;
    Vec4f last_line;
    Vec4f new_line;
    Vec4f left_dx_line;
    Vec4f right_dx_line;
    Vec4f dx_last_line;
    Vec2d detect_pantgraph_info;//输电弓信息
    Vec2d detect_pantgraph_line_info;//输电线信息
    QFile outFile;
    QTextStream datatext;
    Point detect_point;
    vector<Vec3i>N_Point_infos;
    Vec3i Nerborhood_Point_Info;
    int label;
signals:
    void send_dispImage(QImage);
    void send_sendTime(QString);
    void send_ishaveGj(bool);
    void send_templeteImage(QImage);
    void send_singleLineInfo(int x,int y);
    void send_secondLineInofo(Point,Point);
public slots:
    void accept_MatchFileInfo(QString);
    void accept_isTempleteFile(int);
public:
    double myabsd(double a);//double型数据求绝对值
    Vec4f coordiExchanged(Vec4f &pts);//左右坐标转变
    void load_templeteImage();//加载模板文件
    Mat ImageProcess(Mat &oriImage);//图像处理主函数
    QImage convertMatToQImage(Mat &mat);//Mat文件转换成QImage
    Mat RoiImageProcess(Mat &RoiImage,Rect &RoiRect);//目标区域处理
    Vec2d pantographLineDetect(Mat &proImage,Rect &RoiRect);//输电弓检测
    Vec2d pantographDetect(Mat &proImage);//输电弓直线检测
    Point newLineDetect(Mat &grayImage,Mat &tempImage,Rect &RoiRect);//直线检测主函数
    Vec2d m_nLineDetect(Mat &procImage);
    Vec2d m_npantLineDetect(Mat &procImage);
    Mat doubleThreshhold(Mat &InputImage,Mat &OutImage,int minValue,int maxValue);
    void eight_neborhood(Mat &InputImage);
protected:
    void run();
};

#endif // IMAGEPROCTHREAD_H
