// Devoloper name: PenkovGS

#include "NoiseGeneration/Components/MatrixNoise2D.h"

MatrixNoise2D::MatrixNoise2D(const int32 lenght, const int32 width) {
	this->length = lenght;
	this->width = width;
	if (this->length <= 0) {
		this->length = 1;
		UE_LOG(LogTemp, Warning,
			   TEXT("Invalid matrix length corrected (input: length = %d, corrected to: length -> 1)"), this->length);
	}
	if (this->width <= 0) {
		this->width = 1;
		UE_LOG(LogTemp, Warning, TEXT("Invalid matrix length corrected (input: width = %d, corrected to: width -> 1)"),
			   this->width);
	}

	for (int i = 0; i < lenght; i++) {
		NoiseMatrixRow temp_row(width);
		matrix.Add(temp_row);
	}
}

MatrixNoise2D::~MatrixNoise2D() {
}

int32 MatrixNoise2D::GetLength() const {
	return length;
}

int32 MatrixNoise2D::GetWidth() const {
	return width;
}

NoiseMatrixRow &MatrixNoise2D::operator[](int32 index) {
	if (!matrix.IsValidIndex(index)) {
		UE_LOG(LogTemp, Fatal, TEXT("Index exceeds array dimensions"));
	}
	return matrix[index];
}
