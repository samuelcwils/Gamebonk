#include <array>

class interrupts {

    public:
        bool IME;
        std::array <bool, 5> IE;
        std::array <bool, 5> IF;


};
