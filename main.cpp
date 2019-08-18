#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;

cv::Rect rect;
cv::Mat src,mask, bgdModel, fgdModel;
bool init=false;
const char* wintitle = "window";
int number = 0;

void showimage()
{
	cv::Mat result,binmask;
	binmask=cv::Mat(src.size(),CV_8UC1);
	binmask = mask & 1;///相当于制作了一个掩模
	if (init)
	{
		src.copyTo(result,binmask);
	}
	else
	{
		src.copyTo(result);///copyTo与setTo不同
	}
	cv::rectangle(result,rect,cv::Scalar(0,0,255),2,8);////这个必须在result上画
	cv::imshow(wintitle,result);
}

void setimageROI()
{
	mask.setTo(cv::Scalar::all(cv::GC_BGD));
	rect.x = max(0, rect.x);//防止框选是越界，x,y选出界外
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, src.cols - rect.x);///防止长宽越界
	rect.height = min(rect.height, src.rows - rect.y);
	mask(rect).setTo(cv::Scalar::all(cv::GC_PR_FGD));
}

void Onmouse(int event,int x,int y,int flags,void* param)///不能有其他多余参数
{ 
	switch (event)
	{
	case cv::EVENT_LBUTTONDOWN:
		rect.x = x;
		rect.y = y;
		rect.width = 1;
		rect.height = 1;
		init = false;///保证每一次重新选框后都重新分割
		number = 0;///保证每一次重新选框都重新置0
		break;

	case cv::EVENT_MOUSEMOVE:
		if (flags&cv::EVENT_FLAG_LBUTTON)
		{
			rect = cv::Rect(cv::Point(rect.x,rect.y),cv::Point(x,y));
			showimage();
		}
		break;

	case cv::EVENT_LBUTTONUP:
		if (rect.width>1&&rect.height>1)
		{
			setimageROI();
			showimage();
		}		
		break;

	default:
		break;
	}
}
void grabcutimage() 
{
	if (rect.width<2&&rect.height<2)
	{
		return;
	}
	if (init)
	{
		cv::grabCut(src, mask, rect, bgdModel, fgdModel, 1);
	}
	else
	{
		cv::grabCut(src,mask,rect,bgdModel,fgdModel,1,cv::GC_INIT_WITH_RECT);
		init = true;
	}
}

int main(int agrc, char** ahrv)
{
	src = cv::imread("D:\\tu\\flower1.jpg");
	if (src.empty())
	{
		printf("conot load the image");
	}
	mask = cv::Mat(src.size(),CV_8UC1);
	mask.setTo(cv::Scalar::all(cv::GC_BGD));
	cv::namedWindow(wintitle,cv::WINDOW_AUTOSIZE);
	cv::imshow(wintitle,src);
	cout << "请框选图像" << endl;
	cv::setMouseCallback(wintitle,Onmouse,0);
	cout << "每按一次n分割一次" << endl;
	cout << "按esc退出分割" << endl;
	
	while (true)
	{
		char c = (char)cv::waitKey(0);
		if (int(c)==27)
		{
			break;
		}
		if (c == 'n')
		{
			number++;
			printf("分割第 %d 次\n", number);
			grabcutimage();
			showimage();
		}
	}
	cv::waitKey(0);
	return 0;
}