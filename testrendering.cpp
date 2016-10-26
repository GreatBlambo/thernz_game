// Backend
struct UploadNUniforms;  
struct DrawIndexed;  
struct DrawIndexedInstanced;

class IBackend
{
#define TZ_BACKEND_FUNC(type) virtual void dispatch(type* data) = 0
 public:
  TZ_BACKEND_FUNC(UploadNUniforms);
  TZ_BACKEND_FUNC(DrawIndexed);
  TZ_BACKEND_FUNC(DrawIndexedInstanced);
};

#define TZ_DISPATCH_FUNC(type) static void function(IBackend* backend, type* data);

// Backend types
struct UploadNUniforms
{
  TZ_DISPATCH_FUNC(UploadNUniforms)
  bool transpose;
  int location;
  size_t n;
  enum Type
	{
	  INT,
	  FLOAT,
	  VEC2,
	  VEC3,
	  VEC4,
	  MAT4
	} type;    
};
  
struct DrawIndexed
{
  TZ_DISPATCH_FUNC(DrawIndexed)
  VertexArrayID vao;
  DrawType draw_type;
    
  size_t start_index;
  size_t num_indices;
  DataType indices_type;

  Material material;
};
  
struct DrawIndexedInstanced
{
  TZ_DISPATCH_FUNC(DrawIndexedInstanced)
  VertexArrayID vao;
  DrawType draw_type;
    
  size_t start_index;
  size_t num_indices;
  size_t instances;
  DataType indices_type;

  Material material;
};
