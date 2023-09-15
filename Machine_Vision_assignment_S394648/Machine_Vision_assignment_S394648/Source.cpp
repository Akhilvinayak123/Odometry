//call all the libraries required
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <vector>


//specifying the namespaces used in the program
using namespace std;
using namespace cv;


//Creating class 
class ClickableImage
{
private:
    Mat image;  // variable to store the input image
    Point pt{ -1, -1 }; //variable to store the last clicked point
    bool newCoords = false; //flag to track if new coordinates are added
    vector<Point2f> points; //vector to store all the left clicked points
    vector<Point2f> Rpoints;    //vector to store all the right clicked points
    vector<double> distances;   //vector to store the distances between consecutive points
    double conversion_factor;   //variable to store the conversion factor

public:

    //constructor to initialize the ClickableImage object with input image
    ClickableImage(Mat input_image)
    {

        image = input_image;
    }


    //Mouse callback function to handle mouse events on the image
    static void mouse_callback(int event, int x, int y, int flags, void* param)
    {

        ClickableImage* self = static_cast<ClickableImage*>(param); //converting void pointer (param) to a ClickableImage pointer
        double dist_mm = 22.5; //the diameter of coin
        // Check if the mouse click is inside the image boundaries
        if (x < 0 || x >= self->image.cols || y < 0 || y >= self->image.rows)
        {
            cerr << "Error: Click is outside the image boundaries" << endl;
            return;
        }
        if (event == EVENT_LBUTTONDOWN)
        {
            // Add a point on  left-click 
            Point2f pt2f(x, y);
            self->points.push_back(pt2f); //adds coordinates of point where the user clicked on the image to the vector of points stored in 'ClickableImage' object
            if (self->points.size() < 1)
            {
                cerr << "Error: Please click another point" << endl;
                return;
            }
            if (self->points.size() > 1)
            {
                // Calculate distance from last point
                double dist_pixels = norm(self->points.back() - self->points[self->points.size() - 2]);
                self->conversion_factor = dist_pixels / dist_mm; // set the conversion factor value
                self->distances.push_back(dist_mm);
                cout << "Clicked coordinates:" << pt2f << ", Distances from last point:" << dist_mm << "mm" << endl;
                cout << "Clicked coordinates:" << pt2f << ", Distances from last point:" << dist_pixels << "pixels" << endl;
                cout << "Conversion_Factor:" << self->conversion_factor << endl;
            }
            self->newCoords = true;
        }

        else if (event == EVENT_RBUTTONDOWN)
        {
            //Add a point on right-click
            Point2f pt2f(x, y);
            self->Rpoints.push_back(pt2f);

            if (self->Rpoints.size() > 1)
            {
                //  Calculate Euclidian distances from last point
                double dist_pixels1 = norm(self->Rpoints.back() - self->Rpoints[self->Rpoints.size() - 2]);
                double dist_mm1 = dist_pixels1 / self->conversion_factor; //use the conversion_factor to measure the real distance between two points
                self->distances.push_back(dist_mm1);
                cout << "Clicked coordinates:" << pt2f << ", Distances from last point:" << dist_mm1 << "mm" << endl;
                cout << "Clicked coordinates:" << pt2f << ", Distances from last point:" << dist_pixels1 << "pixels" << endl;
            }
            self->newCoords = true;
        }
        else if (event == EVENT_MBUTTONDOWN)
        {
            // Remove last point using middleButtom-click
            if (!self->points.empty())
            {
                self->points.pop_back();
                if (self->distances.size() > 1)
                {
                    self->distances.pop_back();
                }
                self->newCoords = true;
            }
        }
    }



    // finding the dimensions 
    void display(double dist_mm)
    {
        //Display the image
        namedWindow("img", WINDOW_NORMAL);


        //Set callback
        setMouseCallback("img", &ClickableImage::mouse_callback, this);

        for (;;)
        {
            Mat frame = image.clone();


            //Show last point clicked.
            if (pt.x != -1 && pt.y != -1)
            {
                circle(frame, pt, 3, Scalar(0, 0, 255));

                if (newCoords)
                {
                    newCoords = false;
                }
            }

            //Draw all the previous points and their distances
            for (size_t i = 0; i < points.size(); i++)
            {
                circle(frame, points[i], 3, Scalar(0, 0, 255));
                if (i > 0)
                {
                    Point2f midpoint = (points[i] + points[i - 1]) / 2;
                    double dist_pixels = norm(points[i] - points[i - 1]);

                    putText(frame, to_string(dist_pixels) + "pixels", midpoint, FONT_HERSHEY_TRIPLEX, 2, Scalar(0, 0, 255));
                    line(frame, points[i], points[i - 1], Scalar(255, 0, 0));
                }
            }
            imshow("img", frame);

            // exit if q is pressed
            if ((waitKey(1) & 0xFF) == 'q')
                break;
        }


    }


    //finding the dimensions
    void finalsidisplay(double conversion_factor)
    {
        //display the image
        namedWindow("img", WINDOW_NORMAL);


        //Set callback
        setMouseCallback("img", &ClickableImage::mouse_callback, this);

        for (;;)
        {
            Mat frame = image.clone();


            //Show last point clicked.
            if (pt.x != -1 && pt.y != -1)
            {
                circle(frame, pt, 3, Scalar(0, 0, 255));

                if (newCoords)
                {
                    newCoords = false;
                }
            }

            //Draw all the previous points and their distances 
            for (size_t i = 0; i < Rpoints.size(); i++)
            {
                circle(frame, Rpoints[i], 2, Scalar(0, 0, 255));
                if (i > 0)
                {
                    Point2f midpoint = (Rpoints[i] + Rpoints[i - 1]) / 2;
                    double dist_pixels = norm(Rpoints[i] - Rpoints[i - 1]);
                    double dist_mm = dist_pixels / conversion_factor;

                    putText(frame, to_string(dist_mm) + "mm", midpoint, FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 0, 255));
                    line(frame, Rpoints[i], Rpoints[i - 1], Scalar(255, 0, 0));
                }
            }
            imshow("img", frame);

            //Exit if q is pressed
            if ((waitKey(1) & 0xFF) == 'q')
                break;
        }


    }

    //to store the value of conversion_factor
    double getConversionFactor() const
    {
        return conversion_factor;
    }
};

int main()
{


    cout << "Left Click for calibration, press 'q'  &  then Right click for measuring" << endl;


    Mat img = imread("2X_COIN.jpg"); //read the image
    if (img.empty())
    {
        cerr << "Error: Could not read the image file" << endl;
        exit(1);
    }



    ClickableImage clickableimage(img); //pass the image to the class
    clickableimage.display(22.5); // pass in the diameter in mm for 1 pound coin

    clickableimage.finalsidisplay(clickableimage.getConversionFactor()); //conversion factor used for measureing the object


    return 0;
}
