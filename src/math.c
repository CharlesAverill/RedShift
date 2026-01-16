#include "types.h"
#include "math.h"

const val sine_table_256[256] = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173,
    176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215,
    218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244,
    245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255,
    255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246,
    245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220,
    218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179,
    176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131,
    128, 124, 121, 118, 115, 112, 109, 106, 103, 100,  97,  93,  90,  88,  85,  82,
     79,  76,  73,  70,  67,  65,  62,  59,  57,  54,  52,  49,  47,  44,  42,  40,
     37,  35,  33,  31,  29,  27,  25,  23,  21,  20,  18,  17,  15,  14,  12,  11,
     10,   9,   7,   6,   5,   5,   4,   3,   2,   2,   1,   1,   1,   0,   0,   0,
      0,   0,   0,   0,   1,   1,   1,   2,   2,   3,   4,   5,   5,   6,   7,   9,
     10,  11,  12,  14,  15,  17,  18,  20,  21,  23,  25,  27,  29,  31,  33,  35,
     37,  40,  42,  44,  47,  49,  52,  54,  57,  59,  62,  65,  67,  70,  73,  76,
     79,  82,  85,  88,  90,  93,  97, 100, 103, 106, 109, 112, 115, 118, 121, 124
};

val sin(val x) {
    return sine_table_256[x];
}

val cos(val x) {
    return sin(x + 64);
}

// https://en.wikipedia.org/wiki/Integer_square_root#Algorithm_using_binary_search
static val L, R;
static unsigned short M;
val isqrt(val x) {
    L = 0;
    R = x + 1;

    while (L != R - 1) {
        M = (L + R) / 2;
        if (M * M <= x)
            L = M;
        else
            R = M;
    }

    return L;
}

signed short isqrt16(signed short x) {
    static signed short L, R, M;
    L = 0;
    R = x + 1;

    while (L != R - 1) {
        M = (L + R) / 2;
        if (M * M <= x)
            L = M;
        else
            R = M;
    }

    return L;
}

val quadrant;
sbigval abs_x, abs_y;
val low, high, mid, best_angle, i;
sbigval best_error, error;
sbigval lhs, rhs;
val atan2(sbigval y, sbigval x) {
    // Handle zero cases
    if (x == 0 && y == 0) return 0;
    if (x == 0) return (y > 0) ? 64 : 192; // 90° or 270°
    if (y == 0) return (x > 0) ? 0 : 128;   // 0° or 180°
    
    // Determine quadrant
    abs_x = (x > 0) ? x : -x;
    abs_y = (y > 0) ? y : -y;
    
    if (x > 0 && y > 0) quadrant = 0;      // Q1: 0-64
    else if (x < 0 && y > 0) quadrant = 1; // Q2: 64-128
    else if (x < 0 && y < 0) quadrant = 2; // Q3: 128-192
    else quadrant = 3;                      // Q4: 192-256
    
    // Binary search in first quadrant (0-64)
    // We're looking for angle where tan(angle) = y/x
    low = 0;
    high = 64;
    best_angle = 0;
    best_error = 32767;
    
    for (i = 0; i < 8; ++i) { // 8 iterations gives ~0.5° accuracy
        mid = (low + high) / 2;
        
        // Compare y/x with sin(mid)/cos(mid)
        // Avoid division: y * cos(mid) vs x * sin(mid)
        lhs = ((sbigval)abs_y * (sbigval)(cos(mid) - 128)) / 128;
        rhs = ((sbigval)abs_x * (sbigval)(sin(mid) - 128)) / 128;
        
        error = lhs - rhs;
        if (error < 0) error = -error;
        
        if (error < best_error) {
            best_error = error;
            best_angle = mid;
        }
        
        if (lhs < rhs)
            low = mid + 1;
        else
            high = mid;
    }
    
    // Map to correct quadrant
    if (quadrant == 0) return best_angle;
    else if (quadrant == 1) return 128 - best_angle;
    else if (quadrant == 2) return 128 + best_angle;
    else return 256 - best_angle;
}
