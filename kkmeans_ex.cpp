#include <iostream>
#include <vector>
#include <fstream>

#include <dlib/clustering.h>
#include <dlib/rand.h>
#include <dlib/image_io.h>
#include <dlib/generic_image.h>

#include "version.h"

using namespace std;
using namespace dlib;

typedef matrix<double,2,1> sample_type;

void generators(std::vector<sample_type> &samples)
{
    sample_type m;
    dlib::rand rnd;

    // we will make 50 points from each class
    const long num = 200;

    auto circle_generator = [&samples, num, &rnd, &m](double radius, double offset1, double offset2)
    {
        for (long i = 0; i < num; ++i)
        {
            double sign = 1;
            if (rnd.get_random_double() < 0.5)
                sign = -1;
            m(0) = 2*radius*rnd.get_random_double()-radius;
            m(1) = sign*sqrt(radius*radius - m(0)*m(0));

            m(0) += offset1;
            m(1) += offset2;

            // add this sample to our set of samples we will run k-means
            samples.push_back(m);
        }
    };
//    circle_generator(0.5, 0, 0);
//    circle_generator(10, 0, 0);
//    circle_generator(4, 25, 25);

    auto starsky_generator = [&samples, num, &rnd, &m]()
    {
        for (size_t i = 0; i < num; ++i)
        {
            m(0) = 100*rnd.get_random_double()*((rnd.get_random_double() < 0.5) ? -1 : 1);
            m(1) = 100*rnd.get_random_double()*((rnd.get_random_double() < 0.5) ? -1 : 1);

            // add this sample to our set of samples we will run k-means
            samples.push_back(m);
        }
    };

    starsky_generator();

    ofstream of("kkmeans_ex_starsky.txt");
    for(auto &s : samples)
    {
        of << s(0) << ";" << s(1) << "\n";
    }
    of.close();
    samples.clear();

}

int main(int argc, char* argv[])
{
    try
    {
        typedef radial_basis_kernel<sample_type> kernel_type;
        kcentroid<kernel_type> kc(kernel_type(0.1),0.01, 8);
        kkmeans<kernel_type> test(kc);

        std::vector<sample_type> samples;
        std::vector<sample_type> initial_centers;

        generators(samples);

        int n;

        if ((argc > 1) &&
                (!strncmp(argv[1], "-v", 2) || !strncmp(argv[1], "--version", 9)))
        {
            cout << "version " << version() << endl;
            return 0;
        }
        else if (argc == 2)
        {
            n = atoi(argv[1]);
            cout << "kkmeans n: " << n << endl;
        }
        else
        {
            std::cerr << "Usage: kkmeans <n>\n";
            return 1;
        }

        sample_type m;
        string line;
        freopen("kkmeans_ex_starsky.txt", "rt", stdin);
        while(getline(cin, line))
        {
            auto separator = line.find(';');
            m(0) = stod(line.substr(0, separator));
            m(1) = stod(line.substr(++separator, line.length()));

            samples.push_back(m);
        }

        test.set_number_of_centers(n);
        pick_initial_centers(n, initial_centers, samples, test.get_kernel());
        test.train(samples,initial_centers);

        ofstream of("kkmeans_ex_out.txt");
        for(auto &s : samples)
        {
            of << s(0) << ";" << s(1) << ";" << test(s) <<"\n";
        }
        of.close();

        array2d<rgb_pixel> img;
        img.set_size(200, 200);

        image_type

        //save_png(img, "kkmeans.png");
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}


