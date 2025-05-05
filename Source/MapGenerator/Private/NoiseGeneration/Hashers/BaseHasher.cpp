// Devoloper name: PenkovGS

#include "Hashers.h"

BaseHasher::BaseHasher(int32 seed) {
	this->seed = seed;
	grad_type = GradType::Table;
}

int32 BaseHasher::GetSeed() const {
	return seed;
}

void BaseHasher::SetSeed(const int32 new_seed) {
	this->seed = seed;
}

int32 BaseHasher::GetGradientIndex(const int32 hash, const int32 size_table) const {
	uint32 uhash = static_cast<uint32>(hash);
	if (size_table <= 0)
		UE_LOG(LogTemp, Fatal, TEXT("Trying to produce noise using an empty lookup table"));

	return uhash % size_table;
}
