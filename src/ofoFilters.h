#pragma once

#include <optional>
#include <chrono>
#include <iostream>

namespace ofo
{

namespace math 
{
    constexpr double PI_ = 3.14159265358979323846;
}

class FilterIirSimple {
public:
    enum Type {
        LowPass_1OD,
        HighPass_1OD,
    };

private:
    using timestamp_t = std::chrono::high_resolution_clock::time_point;

    std::optional<double> y0, x0;
    timestamp_t t0;
    
    double fc = 0.0;
    Type type = LowPass_1OD;

public:
    FilterIirSimple(double fc, Type type = LowPass_1OD) 
        : type(type)
    { 
        setCutoffFrequency(fc); 
    }
    FilterIirSimple() : FilterIirSimple(1000.0) {}

    void clear() {
        y0.reset();
        x0.reset();
    }

    void setCutoffFrequency(double fcHz) {
        fc = std::clamp(fcHz, 0.1, 20000.0);
    }

    double filter(double x1, double dt) {
        // Handle filter initialization case:
        if (!y0.has_value()) {
            y0 = x1;
            x0 = x1;
            return 0.0;
        }

        switch (type) {
        case LowPass_1OD:
        {
            double alpha = 1.0 - std::exp(-2.0 * ::ofo::math::PI_ * fc * dt);
            y0 = alpha * x1 + (1.0 - alpha) * (*y0);
            //std::cout << alpha << " " << dt << " " << x1 << " " << *y0 << "\n";
            break;
        }
        case HighPass_1OD:
        {
            double gamma = std::exp(-2.0 * ::ofo::math::PI_ * fc * dt);
            y0 = gamma * (*y0 + x1 - (*x0));
            x0 = x1;
            //std::cout << gamma << " " << dt << " " << x1 << " " << *y0 << "\n";
            break;
        }
        default:
            y0 = 0.0;
            break;
        }

        return *y0;
    }

    double filter(double x1) {
        using namespace std::chrono;
        auto t1 = high_resolution_clock::now();
        double dt = duration_cast<microseconds>(t1 - t0).count() / 1000000.0;
        t0 = t1;
        return filter(x1, dt);
    }
};


}
