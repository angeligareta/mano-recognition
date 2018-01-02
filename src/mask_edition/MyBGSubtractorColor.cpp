#include "MyBGSubtractorColor.hpp"

MyBGSubtractorColor::MyBGSubtractorColor( cv::VideoCapture vc )
{
    cap = vc;
    max_samples = MAX_HORIZ_SAMPLES * MAX_VERT_SAMPLES;

    lower_bounds = std::vector<cv::Scalar>( max_samples );
    upper_bounds = std::vector<cv::Scalar>( max_samples );
    means = std::vector<cv::Scalar>( max_samples );

    // Default settings.
    h_low = 100;
    h_up = 100;
    l_low = 100;
    l_up = 100;
    s_low = 12;
    s_up = 100;

    cv::namedWindow( "Trackbars" );

    cv::createTrackbar( "H low:", "Trackbars", &h_low, 100, &MyBGSubtractorColor::Trackbar_func );
    cv::createTrackbar( "H high:", "Trackbars", &h_up, 100, &MyBGSubtractorColor::Trackbar_func );
    cv::createTrackbar( "L low:", "Trackbars", &l_low, 100, &MyBGSubtractorColor::Trackbar_func );
    cv::createTrackbar( "L high:", "Trackbars", &l_up, 100, &MyBGSubtractorColor::Trackbar_func );
    cv::createTrackbar( "S low:", "Trackbars", &s_low, 100, &MyBGSubtractorColor::Trackbar_func );
    cv::createTrackbar( "S high:", "Trackbars", &s_up, 100, &MyBGSubtractorColor::Trackbar_func );
}

void MyBGSubtractorColor::Trackbar_func( int, void* )
{
}

void MyBGSubtractorColor::LearnModel( )
{
    cv::Mat frame, tmp_frame, hsl_frame;
    std::vector<cv::Point> samples_positions;

    cap >> frame;

    // Store the borders of the squares that will be painted.
    for( int i = 0; i < MAX_HORIZ_SAMPLES; i++ ) {
        for( int j = 0; j < MAX_VERT_SAMPLES; j++ ) {
            cv::Point p;
            p.x = frame.cols / 2 + ( -MAX_HORIZ_SAMPLES / 2 + i ) * ( SAMPLE_SIZE + DISTANCE_BETWEEN_SAMPLES );
            p.y = frame.rows / 2 + ( -MAX_VERT_SAMPLES / 2 + j ) * ( SAMPLE_SIZE + DISTANCE_BETWEEN_SAMPLES );
            samples_positions.push_back( p );
        }
    }

    cv::namedWindow( "Cubre los cuadrados con la mano y pulsa espacio" );

    // Show the rectangles in the frame until the space tab is pressed.
    for(;;) {
        flip( frame, frame, 1 );
        frame.copyTo( tmp_frame );

        // Paint the squares.
        for( int i = 0; i < max_samples; i++ ) {
            cv::rectangle( tmp_frame, cv::Rect( samples_positions[i].x, samples_positions[i].y,
                    SAMPLE_SIZE, SAMPLE_SIZE ), cv::Scalar( 0, 255, 0 ), 2 );
        }

        cv::imshow( "Cubre los cuadrados con la mano y pulsa espacio", tmp_frame );

        char c = cv::waitKey( 40 );
        if( c == ' ' ) {
            break;
        }
        cap >> frame;
    }

    cv::destroyWindow( "Cubre los cuadrados con la mano y pulsa espacio" );

    // CODE 1.1 : Get the ROI's and calculate the average of each one. Store them in means array

    // Convert the actual image to HSL Color Model.
    cv::cvtColor( frame, hsl_frame, CV_BGR2HLS );

    // Get Region Of Interest and calculate the mean.
    for( int i = 0; i < max_samples; i++ ) {
        cv::Mat roi = hsl_frame( cv::Rect( samples_positions[i].x, samples_positions[i].y, SAMPLE_SIZE, SAMPLE_SIZE ) ); // Get Sample Square
        means[i] = cv::mean( roi ); // Calculate the pixel's average of the Sample Square.
    }
}

void MyBGSubtractorColor::ObtainBGMask( cv::Mat frame, cv::Mat &bgmask )
{
    // CODE 1.2: For each mean, get the range of each HSL channel.
    cv::Mat hsl_frame;
    cv::cvtColor( frame, hsl_frame, CV_BGR2HLS );

    // Instance an Image ACC with 8-bit single channel image.
    cv::Mat acc( frame.rows, frame.cols, CV_8UC1, cv::Scalar( 0 ) );

    for( int i = 0; i < max_samples; i++ ) {
        lower_bounds[i][0] = std::max( 0.0, means[i][0] - h_low );
        lower_bounds[i][1] = std::max( 0.0, means[i][1] - l_low );
        lower_bounds[i][2] = std::max( 0.0, means[i][2] - s_low );

        upper_bounds[i][0] = std::min( 255.0, means[i][0] + h_up );
        upper_bounds[i][1] = std::min( 255.0, means[i][1] + l_up );
        upper_bounds[i][2] = std::min( 255.0, means[i][2] + s_up );

        cv::inRange( hsl_frame, lower_bounds[i], upper_bounds[i], bgmask );
        acc += bgmask;
    }

    acc.copyTo( bgmask );
}
