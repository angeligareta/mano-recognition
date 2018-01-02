#include "mask_edition/MyBGSubtractorColor.hpp"
#include "gestures/HandGesture.hpp"

int main( int argc, char** argv )
{
    // Mat represents an n-dimensional numerical single-channel or multi-channel array. 
    // It can stores images.
    cv::Mat frame
          , bgmask
          , draw;

    int finger_number = -1;
    bool draw_mode = false;
    auto random_color = cv::Scalar( 72, 98, 235 );
    double previous_area = 0.0;
    cv::Point previous_point = {-1, -1};

    // Para detectar el movimiento del usuario al pintar
    std::vector<int> movements = {0, 0, 0}
                   , expectedMoves = {-1, 1, -1};
    int moveIndex = 0, currentMove = 0;

    // Open the 1º webcam in the system.
    cv::VideoCapture cap( 0 );
    if( !cap.isOpened( ) ) {
        printf( "\nNo se puede abrir la cámara\n" );
        return -1;
    }

    // We wait until we get a valid frame.
    int cont = 0;
    while( frame.empty( ) && ( cont < 2000 ) ) {
        cap >> frame;
        ++cont;
    }
    if( cont >= 2000 ) {
        printf( "No se ha podido leer un frame válido\n" );
        exit( -1 );
    }

    // Create the windows we will be using
    cv::namedWindow( "Reconocimiento" );
    cv::namedWindow( "Fondo" );
    cv::namedWindow( "Dibujo" );

    // Create Substractor
    MyBGSubtractorColor substractor( cap );

    // Create HandGesture Object
    HandGesture detection;

    // Get the model of the frame.
    substractor.LearnModel( );

    // We will be capturing new frames until 'q' is pressed.
    for(;; ) {
        // Capture new frame.
        cap >> frame;
        // flip(frame, frame, 1);

        if( frame.empty( ) ) {
            printf( "Leído frame vacío\n" );
            continue;
        }

        int c = cv::waitKey( 40 );
        if( (char) c == 'q' ) break;

        // Get the backround mask of the actual frame.
        substractor.ObtainBGMask( frame, bgmask );

        // CODIGO 2.1: Reduce mask noise.
        cv::medianBlur( bgmask, bgmask, 5 );

        int dilation_size = 2;
        cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                cv::Size( 2 * dilation_size + 1, 2 * dilation_size + 1 ),
                cv::Point( dilation_size, dilation_size ) );

        cv::dilate( bgmask, bgmask, element );

        // deteccion de las características de la mano
        double countour_area;
        std::vector<cv::Point> resulting_points = detection.FeaturesDetection( bgmask, frame, countour_area );

        // Primero vemos el número de puntos que hay
        if( finger_number != resulting_points.size( ) ) {
            finger_number = resulting_points.size( );

            // Empezamos o Acabamos el modo dibujo si tiene los 5 dedos
            // O si el usuario ha pintado siguiendo el patrón de 
            // movimientos derecha-izquierda-derecha
            if( finger_number == 4 || movements == expectedMoves) {
                movements = {0, 0, 0};
                moveIndex = 0;
                currentMove = 0;

                // Si ya estabamos jugando acabamos
                if( draw_mode ) {
                    draw_mode = false;
                    draw.setTo( cv::Scalar( 255, 255, 255 ) );
                    previous_point = {-1, -1};
                    cv::putText( frame, "Juego finalizado. Cierre y abra para jugar de nuevo.", cv::Point( 50, 50 ), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB( 255, 0, 0 ), 2.0 );
                }
                // Si no estabamos jugando
                else {
                    draw_mode = true;
                    frame.copyTo( draw );
                    draw.setTo( cv::Scalar( 255, 255, 255 ) );
                    previous_point = {-1, -1};
                }
            }
        }

        // Si ya ha puesto dos dedos empiezo a dibujar
        if( draw_mode ) {
            // Pintar punto y linea
            if( finger_number == 1 ) {
                cv::circle( frame, resulting_points[0], 8, cv::Scalar( 255, 60, 100 ), 3 );
                cv::circle( draw, resulting_points[0], 2, random_color, 3 );
                if( previous_point == cv::Point(-1, -1) ) {
                    previous_point = resulting_points[0];
                }
                auto result = resulting_points[0] - previous_point;
                if( cv::norm( result ) > 20 ) { // Euclidian distance

                    std::string message = "";

                    // Hallamos el movimiento del usuario
                    // Si hay un cambio de movimiento lo guardamos en el vector
                    if (result.x < 0){
                        message += "Te moviste a la derecha ";
                        if (currentMove != -1) {
                            currentMove = -1;
                            movements[moveIndex%movements.size()] = currentMove;
                            moveIndex += 1;
                        }
                    }
                    else if(result.x > 0){
                        message += "Te moviste a la izquierda ";
                        if( currentMove != 1 ){
                            currentMove = 1;
                            movements[moveIndex%movements.size()] = currentMove;
                            moveIndex += 1;
                        }
                    }

                    cv::putText( frame, message, cv::Point( 50, 100 ), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB( 255, 0, 0 ), 2.0 );
                    
                    cv::line( draw, resulting_points[0], previous_point, random_color, 5, CV_AA );
                    previous_point = resulting_points[0];
                }            
            }
            // Cambiar de color si hay un cambio brusco del área de la mano
            // o si el usuario movio la mano derecha-izquierda-derecha
            else if( std::abs( countour_area - previous_area ) < ( 0.01 * std::min(countour_area, previous_area) ) ) {
                int random_red = rand( ) % 256;
                int random_green = rand( ) % 256;
                int random_blue = rand( ) % 256;
                random_color = cv::Scalar( random_blue, random_green, random_red );
            } 
            // Mostrar al usuario ayuda
            else {
                cv::putText( draw, "Use dos dedos para dibujar", cv::Point( 50, 50 ), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB( 255, 0, 0 ), 2.0 );
            }
        }
        else {
            cv::putText( frame, "Juego finalizado. Cierre y abra para jugar de nuevo.", cv::Point( 50, 50 ), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB( 255, 0, 0 ), 2.0 );
        }

        // Mostramos el número de dedos
        cv::putText( frame, "Numero dedos: " + std::to_string( finger_number + 1 ), cv::Point( 50, 50 ), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB( 255, 0, 0 ), 2.0 );

        previous_area = countour_area;

        // mostramos el resultado de la sobstracci�n de fondo
        cv::imshow( "Fondo", bgmask );

        // mostramos el resultado del reconocimento de gestos
        cv::imshow( "Reconocimiento", frame );

        // mostramos el resultados el dibujo SÓLO si estamos en modo dibujo
        if( draw_mode ) {
            cv::imshow( "Dibujo", draw );
        }
    }

    cv::destroyWindow( "Reconocimiento" );
    cv::destroyWindow( "Fondo" );
    cv::destroyWindow( "Dibujo" );
    cap.release( );
    return 0;
}
