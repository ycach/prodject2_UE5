// Devoloper name: PenkovGS

#include "Meshes/FlatMesh.h"
#include "Async/Async.h"

FlatMesh::FlatMesh(uint32 width, uint32 length, uint16 poligon_size, FVector start_pos)
	: BaseMesh(width, length, poligon_size, start_pos) {

}

FlatMesh::~FlatMesh() {
}

void FlatMesh::GenerationGrid() {
	auto thread_grid = Async(EAsyncExecution::Thread, [&]() { GenerateGrid(); });
	auto thread_uv = Async(EAsyncExecution::Thread, [&]() { GenerateUV(); });

	thread_uv.Wait();
	thread_grid.Wait();
}


void FlatMesh::GenerateUV() {
	double aspect_ratio = (double)poligons_in_length / poligons_in_width;
	for (uint32 i = 0; i < poligons_in_length + 1; i++) {
		for (uint32 j = 0; j < poligons_in_width + 1; j++) {
			uv.Add(
				FVector2D(1.0 / (poligons_in_width)*i * scale_uv, 1.0 / (poligons_in_length)*j * aspect_ratio*scale_uv));
		}
	}
}

void FlatMesh::GenerateGrid() {
	ParallelFor(poligons_in_length + 1, [&](int32 index) {
		for (uint32 i = 0; i < poligons_in_width + 1; i++) {
			TSharedPtr<Vertix> temp_vertix = MakeShared<Vertix>(index * (poligons_in_width+1) + i, index * poligon_size,
								   i * poligon_size, 0);
			vertixes[index].Add(temp_vertix);
		}
	});
	ParallelFor(poligons_in_length, [&](int32 index) {
		for (uint32 i = 0; i < poligons_in_width; i++) {
			TSharedPtr<MeshTriangle> temp_triangle1 =
				MakeShared<MeshTriangle>(vertixes[index][i], vertixes[index][i + 1], vertixes[index + 1][i + 1]);
			TSharedPtr<MeshTriangle> temp_triangle2 =
				MakeShared<MeshTriangle>(vertixes[index][i], vertixes[index + 1][i + 1], vertixes[index + 1][i]);
			triangles[index].Add(temp_triangle1);
			triangles[index].Add(temp_triangle2);
		}
	});
}
