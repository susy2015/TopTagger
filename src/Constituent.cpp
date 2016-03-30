#include "Constituent.h"

Constituent::Constituent() : bTagDisc_(0.0) {}

Constituent::Constituent(const TLorentzVector& p, const double& bTagDisc) : p_(p), bTagDisc_(bTagDisc) {}
