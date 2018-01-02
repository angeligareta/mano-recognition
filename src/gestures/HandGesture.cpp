#include "HandGesture.hpp"

HandGesture::HandGesture( )
{
}

double HandGesture::getAngle( cv::Point start_point, cv::Point end_point, cv::Point furthest_point )
{
    double v1[2], v2[2];
    v1[0] = start_point.x - furthest_point.x;
    v1[1] = start_point.y - furthest_point.y;

    v2[0] = end_point.x - furthest_point.x;
    v2[1] = end_point.y - furthest_point.y;

    double ang1 = atan2( v1[1], v1[0] );
    double ang2 = atan2( v2[1], v2[0] );

    double angle = ang1 - ang2;
    if( angle > CV_PI ) angle -= 2 * CV_PI;
    if( angle < -CV_PI ) angle += 2 * CV_PI;

    return (angle * 180.0 / CV_PI );
}

std::vector<cv::Point> HandGesture::FeaturesDetection( cv::Mat mask, cv::Mat output_img, double &countour_area )
{
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Point> resulting_points;

    cv::Mat temp_mask;
    mask.copyTo( temp_mask );

    // CODE 3.1: Detección del contorno de la mano y selección del contorno más largo

    // Encontrar contornos.
    findContours( temp_mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );
    if( contours.size( ) == 0 ) {
        return { };
    }

    // Buscar el contorno mas largo.
    int max_contour_index = 0;
    for( int i = 1; i < contours.size( ); ++i ) {
        if( contours[max_contour_index].size( ) < contours[i].size( ) ) {
            max_contour_index = i;
        }
    }

    // Pintar el contorno.
    drawContours( output_img, contours, max_contour_index,
            cv::Scalar( 255, 0, 0 ), 2, 8,
            std::vector<cv::Vec4i>( ), 0, cv::Point( ) );

    //obtener el convex hull
    std::vector<int> hull;
    convexHull( contours[max_contour_index], hull );

    // pintar el convex hull
    cv::Point pt0 = contours[max_contour_index][hull[hull.size( ) - 1]];
    for( int i = 0; i < hull.size( ); i++ ) {
        cv::Point pt = contours[max_contour_index][hull[i]];
        line( output_img, pt0, pt, cv::Scalar( 0, 0, 255 ), 2, CV_AA );
        pt0 = pt;
    }

    cv::Rect bounding_rect = cv::boundingRect( contours[max_contour_index] );
    cv::rectangle( output_img, bounding_rect.tl( ), bounding_rect.br( ), cv::Scalar( 0, 255, 0 ), 2, 8, 0 );
    countour_area = bounding_rect.area();

    //obtener los defectos de convexidad
    std::vector<cv::Vec4i> defects;
    convexityDefects( contours[max_contour_index], hull, defects );

    // Dibujar los puntos de la mano
    for( int i = 0; i < defects.size( ); i++ ) {
        cv::Point start_point = contours[max_contour_index][defects[i][0]]; // Punto de start (mas cercano atras en la malla convexa)
        cv::Point end_point = contours[max_contour_index][defects[i][1]]; // Punto de end (mas cercano delante en la malla convexa)
        cv::Point furthest_point = contours[max_contour_index][defects[i][2]]; // Punto furthest (Punto mas separado de los dos anteriores, formando un ángulo)
        cv::Point middle_point;
        middle_point.x = ( start_point.x + end_point.x ) / 2;
        middle_point.y = ( start_point.y + end_point.y ) / 2;

        float depth = (float) defects[i][3] / 256.0;
        double angle = getAngle( start_point, end_point, furthest_point );

        // CODE 3.2 - Filtrar y mostrar los defectos de convexidad
        // Nuestros dedos sólo pueden llegar a 90º, asi que no mostramos el resto.
        if( angle < 90 ) {
            circle( output_img, furthest_point, 5, cv::Scalar( 0, 255, 0 ), 3 );
            resulting_points.push_back( middle_point );
        }
    }

    return resulting_points;
}
