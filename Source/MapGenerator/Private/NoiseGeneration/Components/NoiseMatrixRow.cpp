// Devoloper name: PenkovGS


#include "NoiseGeneration/Components/NoiseMatrixRow.h"

NoiseMatrixRow::NoiseMatrixRow(int32 length) {
	this->length = length;
	row.SetNumZeroed(length);
}

NoiseMatrixRow::~NoiseMatrixRow() {
}

int32 NoiseMatrixRow::GetLength() const {
	return length;
}

float &NoiseMatrixRow::operator[](int32 index) {
	if (!row.IsValidIndex(index)) {
		UE_LOG(LogTemp, Fatal, TEXT("Index exceeds array dimensions"));
	}
	return row[index];
}
