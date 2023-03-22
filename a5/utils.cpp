#include "utils.hpp"

// bool operator<(const Room &r1, const Room &r2){
//     return r1.guest.priority < r2.guest.priority;
// }

bool operator==(const Room &r1, const Room &r2){
    return r1.guest.id == r2.guest.id;
}