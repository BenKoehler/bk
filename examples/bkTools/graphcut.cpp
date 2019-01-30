/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <algorithm>
#include <array>
#include <cstdio>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <png.h>

#include <bk/GraphCut>

struct MyGrayImage2D
{
    std::array<unsigned int,2> size;
    std::vector<double> values;

    [[nodiscard]] double& operator()(unsigned int x, unsigned int y) {return values[size[0]*y + x];}
    [[nodiscard]] double operator()(unsigned int x, unsigned int y) const {return values[size[0]*y + x];}
    
    void set_size(unsigned int width, unsigned int height)
    {
        size[0] = width;
        size[1] = height;
        values.resize(size[0] * size[1]);
    }
};

struct MyGradientImage2D
{
    std::array<unsigned int,2> size;
    std::vector<std::pair<double,double>> values;

    [[nodiscard]] std::pair<double,double>& operator()(unsigned int x, unsigned int y) {return values[size[0]*y + x];}
    [[nodiscard]] const std::pair<double,double>& operator()(unsigned int x, unsigned int y) const {return values[size[0]*y + x];}

    void set_size(unsigned int width, unsigned int height)
    {
        size[0] = width;
        size[1] = height;
        values.resize(size[0] * size[1]);
    }
};

MyGrayImage2D imageToSegment;

// below: two black images where a few white strokes mark regions inside/outside the target object
MyGrayImage2D inside;
MyGrayImage2D outside;

[[nodiscard]] MyGrayImage2D load_png(std::string_view filepath, bool* success = nullptr)
{
    MyGrayImage2D img;

    if (success != nullptr)
    {
        *success = false;
    }

    FILE* file = std::fopen(filepath.data(), "rb");
    if (!file)
    {
        std::cerr << "load_png error: could not open file" << std::endl;
        return img;
    }

    png_byte header[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    [[maybe_unused]] const size_t dummy = std::fread(header, 1, 8, file);

    if (png_sig_cmp(header, 0, 8))
    {
        std::cerr << "load_png_gray(" << filepath << ") " << "png_sig_cmp(&header, 0, 8)" << std::endl;
        std::fclose(file);
        return img;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!png_ptr)
    {
        std::cerr << "load_png error at png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)" << std::endl;
        std::fclose(file);
        return img;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr)
    {
        std::cerr << "load_png error at png_create_info_struct(png_ptr)" << std::endl;
        std::fclose(file);
        return img;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        std::cerr << "load_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        std::fclose(file);
        return img;
    }

    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    const png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    const png_uint_32 height = png_get_image_height(png_ptr, info_ptr);

    png_read_update_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        std::cerr << "load_png error at " << std::endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        std::fclose(file);
        return img;
    }

    img.set_size(width, height);

    png_bytepp png_data_raw = new png_bytep[height];

    const png_size_t rowsize = png_get_rowbytes(png_ptr, info_ptr);
    for (png_uint_32 y = 0; y < height; ++y)
    { png_data_raw[y] = new png_byte[rowsize]; }

    png_read_image(png_ptr, png_data_raw);
    const png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    const unsigned int stride = color_type == PNG_COLOR_TYPE_GRAY ? 1 : color_type == PNG_COLOR_TYPE_GA ? 2 : color_type == PNG_COLOR_TYPE_RGB ? 3 : 4;

    for (png_uint_32 y = 0; y < height; ++y)
    {
        const png_bytep row = png_data_raw[y];

        for (png_uint_32 x = 0; x < width; ++x)
        {
            const png_bytep val = &(row[x * stride]);

                if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                {
                    // alpha values are ignored for PNG_COLOR_TYPE_GRAY_ALPHA
                    img(x, y) = *val;
                }
                else if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGBA)
                {
                    // - perform RGB to gray conversion
                    // - potential alpha values are ignored for PNG_COLOR_TYPE_RGBA
                    const double r = 0.2126 * val[0];
                    const double g = 0.7152 * val[1];
                    const double b = 0.0722 * val[2];

                    img(x,y) = r + g + b;
                }
        } // for x
    } // for y

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    std::fclose(file);

    for (png_uint_32 y = 0; y < height; ++y)
    { delete[] png_data_raw[y]; }

    delete[] png_data_raw;

    if (success != nullptr)
    {
        *success = true;
    }

    return img;
}

[[maybe_unused]] bool save_png(const MyGrayImage2D& img, std::string_view filepath)
{
        FILE* file = std::fopen(filepath.data(), "wb");

        if (!file)
        {
            std::fclose(file);
            std::cerr << "save_png: png file creation error" << std::endl;
            return false;
        }

        auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

        if (!png_ptr)
        {
            std::fclose(file);
            std::cerr << "save_png error at png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)" << std::endl;
            return false;
        }

        auto info_ptr = png_create_info_struct(png_ptr);

        if (!info_ptr)
        {
            std::fclose(file);
            std::cerr << "save_png error at png_create_info_struct(png_ptr);" << std::endl;
            return false;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            std::fclose(file);
            std::cerr << "save_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
            return false;
        }

        png_init_io(png_ptr, file);

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            std::fclose(file);
            std::cerr << "save_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
            return false;
        }

        png_uint_32 width = static_cast<png_uint_32>(img.size[0]);
        png_uint_32 height = static_cast<png_uint_32>(img.size[1]);
        const int bit_depth = 8;

        png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_write_info(png_ptr, info_ptr);

    const auto[itMinValue, itMaxValue] = std::minmax_element(img.values.begin(), img.values.end());
    const double minVal = *itMinValue;
    const double maxVal = *itMaxValue;
    const double range = maxVal-minVal;

        png_bytepp buffer = new png_bytep[height];

        for (png_uint_32 y = 0; y < height; ++y)
        {
            buffer[y] = new png_byte[width];

            for (png_uint_32 x = 0; x < width; ++x)
            {
                    if (minVal != maxVal)
                    { buffer[y][x] = static_cast<png_byte>(255.0 * (img(x,y) - minVal) / range); }
                    else
                    { buffer[y][x] = static_cast<png_byte>(255); }
            } // for x
        } // for y

        png_write_image(png_ptr, buffer);

        auto clear_buffer = [&]()
        {
            for (png_uint_32 y = 0; y < height; ++y)
            { delete[] buffer[y]; }

            delete[] buffer;

            png_destroy_write_struct(&png_ptr, &info_ptr);
        };

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            clear_buffer();
            std::fclose(file);
            std::cerr << "save_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
            return false;
        }

        png_write_end(png_ptr, nullptr);
        clear_buffer();
        std::fclose(file);

    return true;
}

/**
 * This example passes a gray image to the graph cut.
 * The default weight function based on the image gradient is used internally.
 * The parameter "tolerance" allows for some fine-tuning of the result.
 * The employed image class is a custom format defined above.
 */
void example1()
{
    std::cout << "Running example 1" << std::endl;

    //------------------------------------------------------------------------------------------------------
    // setup 2D graph cut
    //------------------------------------------------------------------------------------------------------
    //bk::GraphCut<2> gc; // single core version
    bk::GraphCutParallel<2> gc; // parallel version

    // image grid size (Vec2)
    //const std::array<unsigned int,2> imgSize{{imageToSegment.size, imageToSegment.values[0].size()}};

    // image scale [1,1] for a Cartesian image
    const std::array<double,2> imgScale{{1.0, 1.0}};

    // min/max pixel value
    const auto[itMinValue, itMaxValue] = std::minmax_element(imageToSegment.values.begin(), imageToSegment.values.end());
    const double minVal = *itMinValue;
    const double maxVal = *itMaxValue;

    // function to access custom image format
    // - the first parameter is your image
    // - the second parameter is a n-dimensional std::array<int, N> with indices
    // - function returns pixel value at the corresponding image position
    constexpr auto fn_img_access = [](const MyGrayImage2D& img, const std::array<int, 2>& id)
    {
        // in this example, the individual elements of the std::array<int, 2> (parameter id)
        // are passed to operator() of the custom image class
        return img(id[0], id[1]);
    };

    constexpr double tolerance = 0.5; // the default value if no tolerance parameter is passed to gc.set_image()
    gc.init_from_intensity_image(imageToSegment, {imageToSegment.size[0], imageToSegment.size[1]}, {imgScale[0], imgScale[1]}, {minVal, maxVal}, fn_img_access, tolerance);

    /*
     * set input (inside / outside)
     */
    for (unsigned int x = 0; x < imageToSegment.size[0]; ++x)
    {
        for (unsigned int y = 0; y < imageToSegment.size[1]; ++y)
        {
            if (inside(x, y) != 0)
            { gc.add_source_node(x, y); }
            else if (outside(x, y) != 0)
            { gc.add_sink_node(x, y); }
        }
    }

    //------------------------------------------------------------------------------------------------------
    // run graph cut
    //------------------------------------------------------------------------------------------------------
    gc.run();

    //------------------------------------------------------------------------------------------------------
    // get resulting segmentation
    //------------------------------------------------------------------------------------------------------
    MyGrayImage2D segmentation;
    segmentation.set_size(imageToSegment.size[0], imageToSegment.size[1]);

    for (unsigned int x = 0; x < imageToSegment.size[0]; ++x)
    {
        for (unsigned int y = 0; y < imageToSegment.size[1]; ++y)
        {
            if (gc.is_in_segmentation(x, y))
            { segmentation(x, y) = 1; }
        }
    }

    save_png(segmentation, "graphcut_output_example1.png");
    std::cout << "segmentation savec to graphcut_output_example1.png" << std::endl;
}

/**
 * This example passes a weight image W with custom edge weights to the graph cut.
 * Each element of W contains one weight per dimension: The edge weight to the next element in the corresponding dimension.
 *
 * For example in 2D, W(0,0) contains a 2D vector where the first element is the weight between (0,0)<->(1,0) and the second element is the weight between (0,0)<->(0,1)
 */
void example2()
{
    std::cout << "Running example 2" << std::endl;

    const std::array<double,2> imgScale{{1.0, 1.0}};

    // 2D weight image with vec2 per element (the gradient)
    MyGradientImage2D gradientBasedWeights;
    gradientBasedWeights.set_size(imageToSegment.size[0], imageToSegment.size[1]);

    #pragma omp parallel for
    for (unsigned int x = 0; x < imageToSegment.size[0]; ++x)
    {
        for (unsigned int y = 0; y < imageToSegment.size[1]; ++y)
        {
            double wx = 0;
            double wy = 0;

            if (x > 0 && x < imageToSegment.size[0]-1)
            {
                const double centralDifference = (imageToSegment(x+1,y) - imageToSegment(x-1,y)) / (2*imgScale[0]);
                wx = std::exp(-std::abs(centralDifference)); // custom weight function
            }

            if (y > 0 && y < imageToSegment.size[1]-1)
            {
                const double centralDifference = (imageToSegment(x,y+1) - imageToSegment(x,y-1)) / (2*imgScale[1]);
                wy = std::exp(-std::abs(centralDifference)); // custom weight function
            }

            gradientBasedWeights(x,y) = std::pair(wx, wy);
        }
    }

    //------------------------------------------------------------------------------------------------------
    // setup 2D graph cut
    //------------------------------------------------------------------------------------------------------
    //bk::GraphCut<2> gc; // single core version
    bk::GraphCutParallel<2> gc; // parallel version

    // function to access custom weight image
    // - the first parameter is your weight image
    // - the second parameter is a n-dimensional std::array<int, N> with indices
    // - the third parameter is the id of the current dimension
    // - function returns the weight at the corresponding image position and dimension
    constexpr auto fn_weight_access = [](const auto& weightImage, const std::array<int, 2>& id, unsigned int dimId)
    {
        // operator() of the weightImage returns a Vec2<double>
        // operator[] of the Vec2<double> returns a double
        const std::pair<double, double>& xy = weightImage(id[0], id[1]);
        return dimId == 0 ? xy.first : xy.second;
    };

    gc.init_from_weight_image(gradientBasedWeights, {imageToSegment.size[0], imageToSegment.size[1]}, fn_weight_access);

    //------------------------------------------------------------------------------------------------------
    // set input (inside / outside)
    //------------------------------------------------------------------------------------------------------
    for (unsigned int x = 0; x < imageToSegment.size[0]; ++x)
    {
        for (unsigned int y = 0; y < imageToSegment.size[1]; ++y)
        {
            if (inside(x, y) != 0)
            { gc.add_source_node(x, y); }
            else if (outside(x, y) != 0)
            { gc.add_sink_node(x, y); }
        }
    }

    //------------------------------------------------------------------------------------------------------
    // run graph cut
    //------------------------------------------------------------------------------------------------------
    gc.run();

    //------------------------------------------------------------------------------------------------------
    // get segmentation
    //------------------------------------------------------------------------------------------------------
    MyGrayImage2D segmentation;
    segmentation.set_size(imageToSegment.size[0], imageToSegment.size[1]);

    for (unsigned int x = 0; x < imageToSegment.size[0]; ++x)
    {
        for (unsigned int y = 0; y < imageToSegment.size[1]; ++y)
        {
            if (gc.is_in_segmentation(x, y))
            { segmentation(x, y) = 1; }
        }
    }

    save_png(segmentation, "graphcut_output_example2.png");
    std::cout << "segmentation savec to graphcut_output_example2.png" << std::endl;
}

int main(int, char**)
{
    /*
     * load input
     */
    std::cout << "Loading images ... ";

    // the image we want to segment
    imageToSegment = load_png("graphcut_input_image.png");

    // below: two black images where a few white strokes mark regions inside/outside the target object
    inside = load_png("graphcut_input_inside.png");
    outside = load_png("graphcut_input_outside.png");

    std::cout << "done!" << std::endl;
    std::cout << "image size: " << imageToSegment.size[0] << " x " << imageToSegment.size[1] << std::endl;
    std::cout << std::endl;

    /*
     * run graph cuts
     */
    example1();
    std::cout << std::endl;

    example2();

    return EXIT_SUCCESS;
}

