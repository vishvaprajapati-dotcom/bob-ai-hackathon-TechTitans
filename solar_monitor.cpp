#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Pixel {
    unsigned char r, g, b;
};

struct Image {
    int width = 0;
    int height = 0;
    std::vector<Pixel> p;
};

static std::string nextToken(std::istream& in) {
    std::string s;
    char c;
    while (in.get(c)) {
        if (c == '#') {
            std::string ignored;
            std::getline(in, ignored);
            continue;
        }
        if (!std::isspace(static_cast<unsigned char>(c))) {
            s += c;
            break;
        }
    }
    while (in.get(c) && !std::isspace(static_cast<unsigned char>(c))) s += c;
    return s;
}

bool loadPPM(const std::string& path, Image& img) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    std::string magic = nextToken(in);
    if (magic != "P6") return false;

    std::string sw = nextToken(in), sh = nextToken(in), sm = nextToken(in);
    if (sw.empty() || sh.empty() || sm.empty()) return false;

    img.width = std::stoi(sw);
    img.height = std::stoi(sh);
    int maxv = std::stoi(sm);
    if (img.width <= 0 || img.height <= 0 || maxv != 255) return false;

    img.p.resize(static_cast<size_t>(img.width) * img.height);
    in.read(reinterpret_cast<char*>(img.p.data()),
            static_cast<std::streamsize>(img.p.size() * sizeof(Pixel)));
    return static_cast<bool>(in);
}

bool savePPM(const std::string& path, const Image& img) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out << "P6\n" << img.width << " " << img.height << "\n255\n";
    out.write(reinterpret_cast<const char*>(img.p.data()),
              static_cast<std::streamsize>(img.p.size() * sizeof(Pixel)));
    return static_cast<bool>(out);
}

double intensity(const Pixel& p) {
    return 0.299 * p.r + 0.587 * p.g + 0.114 * p.b;
}

void drawCircle(Image& img, int cx, int cy, int radius) {
    const double pi = 3.14159265358979323846;
    for (int a = 0; a < 360; ++a) {
        double t = a * pi / 180.0;
        int x = static_cast<int>(std::lround(cx + radius * std::cos(t)));
        int y = static_cast<int>(std::lround(cy + radius * std::sin(t)));

        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -2; dx <= 2; ++dx) {
                int xx = x + dx, yy = y + dy;
                if (xx >= 0 && xx < img.width && yy >= 0 && yy < img.height) {
                    Pixel& q = img.p[static_cast<size_t>(yy) * img.width + xx];
                    q = {255, 0, 0};
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: solar_monitor <input.ppm> <output.ppm> <report.txt>\n";
        return 1;
    }

    Image img;
    if (!loadPPM(argv[1], img)) {
        std::cerr << "Could not read a valid binary P6 PPM image: " << argv[1] << "\n";
        return 2;
    }

    double globalSum = 0.0;
    for (const auto& p : img.p) globalSum += intensity(p);
    const double globalMean = globalSum / img.p.size();

    const int gridX = 8;
    const int gridY = 6;
    double bestScore = -1.0;
    int bestGX = 0, bestGY = 0;

    for (int gy = 0; gy < gridY; ++gy) {
        int y0 = gy * img.height / gridY;
        int y1 = (gy + 1) * img.height / gridY;
        for (int gx = 0; gx < gridX; ++gx) {
            int x0 = gx * img.width / gridX;
            int x1 = (gx + 1) * img.width / gridX;

            double sum = 0.0;
            double sq = 0.0;
            int n = 0;
            for (int y = y0; y < y1; ++y) {
                for (int x = x0; x < x1; ++x) {
                    double v = intensity(img.p[static_cast<size_t>(y) * img.width + x]);
                    sum += v;
                    sq += v * v;
                    ++n;
                }
            }
            if (n == 0) continue;

            double mean = sum / n;
            double variance = std::max(0.0, sq / n - mean * mean);
            double stddev = std::sqrt(variance);

            // Bright or dark regions plus internal variation are suspicious.
            double contrast = std::abs(mean - globalMean);
            double score = contrast + 0.35 * stddev;

            if (score > bestScore) {
                bestScore = score;
                bestGX = gx;
                bestGY = gy;
            }
        }
    }

    int x0 = bestGX * img.width / gridX;
    int x1 = (bestGX + 1) * img.width / gridX;
    int y0 = bestGY * img.height / gridY;
    int y1 = (bestGY + 1) * img.height / gridY;

    int cx = (x0 + x1) / 2;
    int cy = (y0 + y1) / 2;
    int radius = std::max(10, std::min(x1 - x0, y1 - y0) / 3);

    Image annotated = img;
    drawCircle(annotated, cx, cy, radius);

    if (!savePPM(argv[2], annotated)) {
        std::cerr << "Could not write output image.\n";
        return 3;
    }

    std::ofstream report(argv[3]);
    report << "Solar Monitor Detection Report\n";
    report << "==============================\n";
    report << "Image: " << argv[1] << "\n";
    report << "Image size: " << img.width << " x " << img.height << "\n";
    report << "Candidate center: (" << cx << ", " << cy << ")\n";
    report << "Candidate radius: " << radius << " pixels\n";
    report << "Anomaly score: " << bestScore << "\n";
    report << "Grid cell: (" << bestGX << ", " << bestGY << ")\n";
    report << "\nInterpretation: the marked region is a suspicious image anomaly, "
              "not a confirmed physical fault.\n";

    std::cout << "Detection complete.\n";
    std::cout << "Suspicious region center: (" << cx << ", " << cy << ")\n";
    std::cout << "Anomaly score: " << bestScore << "\n";
    std::cout << "Output: " << argv[2] << "\n";
    std::cout << "Report: " << argv[3] << "\n";

    return 0;
}
