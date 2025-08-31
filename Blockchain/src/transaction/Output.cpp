#include "Output.hpp"
#include <sstream>

const std::string Output::toString() const {
    std::ostringstream oss;
    oss << "Value: " << value << ", PubKey: " << pubKey;
    return oss.str();
}
