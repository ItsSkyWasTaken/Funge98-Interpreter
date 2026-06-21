#ifndef FUNGE98_IMTH_HPP
#define FUNGE98_IMTH_HPP

#include "fingerprint.hpp"

class IMathFP : public Fingerprint {
    public:
        /// Returns the static instance of this fingerprint, or creates a new one if it hasn't been created yet.
        ///
        /// @return  the singleton instance of this fingerprint
        static std::shared_ptr<IMathFP> instance();

        IMathFP(FingerprintToken);
        ~IMathFP() override = default;

    // Overridden methods.
    protected:
        bool a(InstructionPointer& ip) override;
        bool b(InstructionPointer& ip) override;
        bool c(InstructionPointer& ip) override;
        bool d(InstructionPointer& ip) override;
        bool e(InstructionPointer& ip) override;
        bool f(InstructionPointer& ip) override;
        bool g(InstructionPointer& ip) override;
        bool h(InstructionPointer& ip) override;
        bool i(InstructionPointer& ip) override;
        bool l(InstructionPointer& ip) override;
        bool n(InstructionPointer& ip) override;
        bool r(InstructionPointer& ip) override;
        bool s(InstructionPointer& ip) override;
        bool t(InstructionPointer& ip) override;
        bool u(InstructionPointer& ip) override;
        bool x(InstructionPointer& ip) override;
        bool z(InstructionPointer& ip) override;

    private:
        /// The singleton instance of this fingerprint.
        inline static std::shared_ptr<IMathFP> inst = nullptr;
};

#endif
