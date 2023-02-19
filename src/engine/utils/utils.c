#include <utils.h>

i08 file_exists(const char * path) {
    assert(path != NULL);
    FILE * f = fopen(path, "rb");
    if (f) {
        return 1;
    }
    return 0;
}

// Maximum readable size is 2 GB
file_info_t * txt_file_query(const char * path) {
    assert(path != NULL);
    FILE * f = fopen(path, "rb");
    assert(f != NULL);

    file_info_t * file = calloc(1, sizeof(file_info_t));
    file -> path = path;

    assert(fseek(f, 0, SEEK_END) == 0);

    file -> len = ftell(f);
    assert(file -> len != 0);

    file -> content = calloc(file -> len + 1, sizeof(char));
    assert(file -> content != NULL);
    
    assert(fseek(f, 0, SEEK_SET) == 0);
    
    assert(fread(file -> content, sizeof(char), file -> len, f) == file -> len);
    file -> content[file -> len + 1] = '\0';
    
    assert(fclose(f) == 0);
    return file;
}

// It's not possible to write binary data, or more than 2GB, using write_file.
i08 txt_file_write(const char * content, const char * path, const size_t len) { 
    assert(content != NULL && path != NULL && len > 0);
    FILE * f = fopen(path, "wb");
    assert(f != NULL);
    assert(fwrite(content, sizeof(char), len, f) == len);
    assert(fclose(f) == 0);
    return 1;
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
u64 hash_id(const char * id) {
    assert(id != NULL);
    u64 hash = FNV_OFFSET;
    for (const char * p = id; *p; p++) {
        hash ^= (u64)(u08)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

// Capacity MUST BE power of 2
void increase_ht_capacity(ht_t * ht, u32 capacity) {
    assert(ht != NULL && capacity % 2 == 0);
    ht_t new_ht;
    new_ht.data = calloc(capacity, sizeof(ht_item_t));
    assert(new_ht.data != NULL);
    new_ht.len = 0;
    new_ht.capacity = capacity;
    for (u64 i = 0; i < ht -> capacity; i++) {
        if (ht -> data[i] != NULL) {
            ht_insert(&new_ht, ht -> data[i] -> id, ht -> data[i] -> val);
        }
    }

    ht = &new_ht;
}

// - Add collision handling -> linear probing 
// - More functionality:
//  - Add          O(1)
//  - Remove       O(1) 
//  - Get          O(1)
//  - Expand       O(n)
// - Good hash function

// Capacity MUST BE power of 2
ht_t ht_create(const u32 capacity) {
    assert(capacity % 2 == 0);
    ht_t table;
    table.data = calloc(capacity, sizeof(ht_item_t));
    assert(table.data != NULL); 
    table.len = 0;
    table.capacity = capacity;
    return table;
}

u64 ht_insert(ht_t * ht, const char * id, void * data) {
    assert(ht != NULL && id != NULL && data != NULL);
    ht_item_t * item = calloc(1, sizeof(ht_item_t));
    assert(item != NULL);
    item -> hash = hash_id(id);
    item -> id = id;
    item -> val = data;
    u64 index = item -> hash & (u64)(ht -> capacity - 1);

    ht_item_t * current = ht -> data[index];
    while (current != NULL) {
        index++;
        current = ht -> data[index];
    }

    if (index > ht -> capacity) {
        increase_ht_capacity(ht, ht -> capacity * 2);
        ht -> data[index] = item;
    }

    if (ht -> data[index] == NULL) {
        ht -> data[index] = item;
    } 

    ht -> len++;   
    return index;
} 

void * ht_get(const ht_t * ht, const char * id) {
    assert(ht != NULL && id != NULL);
    u64 index = hash_id(id) & (u64)(ht -> capacity - 1); 
    ht_item_t * data = ht -> data[index];
     
    if (data == NULL) {
        return NULL;
    }

    if (data -> val == NULL) {
        return NULL;
    }

    while (strcmp(data -> id, id) != 0) {
        index++; 
        data = ht -> data[index]; 
    }

    if (data -> val != NULL && strcmp(data -> id, id) == 0) {
        return data -> val; 
    }

    return NULL;
}


u64 ht_rm_data(ht_t * ht, const char * id) {
    assert(ht != NULL && id != NULL);
    u64 index = hash_id(id) & (u64)(ht -> capacity - 1); 
    ht_item_t * data = ht -> data[index];
    
    if (data -> val == NULL) {
        return 0;
    }

    while (strcmp(data -> id, id) != 0) {
        index++; 
        data = ht -> data[index]; 
    }

    if (data -> val != NULL && strcmp(data -> id, id) == 0) {
        data = NULL;
        ht -> data[index] = NULL;
    }
    return index;
}


void ht_rm(ht_t * ht) {
    assert(ht != NULL);
    for (u32 i = 0; i < ht -> capacity; i++) {
        free(ht -> data[i]);
    }
    free(ht -> data);   
}

list_t list_create(const u32 capacity) {
    list_t list;
    list.data = calloc(capacity, sizeof(void*)); 
    assert(list.data != NULL);
    list.capacity = capacity;
    list.len = 0;
    return list;
}

void list_insert(list_t * list, void * data) {
    assert(list != NULL && data != NULL);
    if (list -> len + 1 > list -> capacity) {   
        list -> capacity += 10;
        void * new_data = realloc(list -> data, list -> capacity);
        assert(new_data != NULL);
        list -> data = new_data;
    }
    list -> data[list -> len] = data;
    list -> len++; 

}

i32 list_get_len_of(const list_t list, void * data) {
    assert(data != NULL);
    for (i32 i = 0; i < list.capacity - 1; i++) {
        if (list.data[i] == data) {
            return i;
        }
    }

    return 0x0;
}

void list_rm_at(list_t * list, u32 len) {
    assert(list != NULL);
    while (len + 1 < list -> capacity) {
        list -> data[len] = list -> data[len + 1]; 
    }      
}

void list_rm_data(list_t * list, void * data) {
    assert(list != NULL && data != NULL);
    u32 len = list_get_len_of(*list, data);
    list_rm_at(list, len);
}

void list_rm(list_t * list) {
    assert(list != NULL);
    for (i32 i = 0; i < list -> capacity - 1; i++) {
        free(list -> data[i]);
    } 

    free(list -> data);
}

#ifdef __EMSCRIPTEN
    #define NO_ANSI
#endif

char * type[LOG_LEVEL_LEN] = {
    "DEBUG",
    "SUCCESS",
    "WARN",
    "ERR"
}; 

char * colors[LOG_LEVEL_LEN] = {
    "\x1b[0m",
    "\x1b[32m",
    "\x1b[1;33m",
    "\x1b[31m"
};

time_t current_time;
struct tm * m_time; 

void get_time() {
    time(&current_time);
    m_time = localtime(&current_time);
}

void logger_log(LOG_LEVEL level, const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
        get_time();
        #ifndef NO_ANSI 
            printf("%s", colors[level]);
        #endif
        
        printf("[%d/%d/%d -> %d:%d:%d][%s] ", m_time -> tm_mday,
                                        m_time -> tm_mon,
                                        m_time -> tm_year + 1900,
                                        m_time -> tm_hour, 
                                        m_time -> tm_min, 
                                        m_time -> tm_sec,
                                        type[level]); 
        vfprintf(stdout, fmt, args);
        printf("\n%s", colors[LOG_DBG]);
    va_end(args);
}

f32 dist2D(f32 x1, f32 y1, f32 x2, f32 y2) {
    return sqrt(sq(x2 - x1) + sq(y2 - y1));
}

f32 dir2D(f32 x1, f32 y1, f32 x2, f32 y2) {
    return atan2(y2 - y1, x2 - x1);  
}

f32 ilerp(f32 start, f32 stop, f32 amt) {
    return start + (stop - start) * amt; 
}

i64 randnum_gen(i64 lower, i64 upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

v2_t v2_add(v2_t v1, v2_t v2) {
    return mk_v2(v1.x + v2.x, v1.y + v2.y);
}

v2_t v2_sub(v2_t v1, v2_t v2) {
    return mk_v2(v2.x - v1.x, v2.y - v1.y);
}

v2_t v2_lerp(v2_t v1, v2_t v2, f32 amt) {
    v2_t vec;
    vec.x = ilerp(v1.x, v2.x, amt);
    vec.y = ilerp(v1.y, v2.y, amt);
    return vec;
}


void v2_scale(v2_t * v, f32 scalar) {
    assert(v != NULL);
    v -> x *= scalar;
    v -> y *= scalar;
}

void v2_normalize(v2_t * v) {
    assert(v != NULL);
    f32 mag = v2_mag(*v);

    if (mag != 1.0f) {
        v -> x = v -> x / mag;
        v -> y = v -> y / mag;  
    }
}


void v2_limit(v2_t * v, f32 scalar) {
    assert(v != NULL);
    f32 len = v -> x * v -> x + v -> y * v -> y;
    f32 len_t = scalar * scalar;
    if (len > len_t) {
        v2_normalize(v);
        v2_scale(v, scalar);
    }         
}

void v2_rotate(v2_t * v, f32 theta) {
    assert(v != NULL);
    f32 temp = v -> x;
    v -> x = temp * cosf(theta) - v -> y * sinf(theta);
    v -> y = temp * sinf(theta) + v -> y * cosf(theta);
}

void v2_mag_set(v2_t * v, f32 scalar) {
    assert(v != NULL);
    v2_normalize(v);
    v2_scale(v, scalar);
}

void v2_negate(v2_t * v) {
    assert(v != NULL);
    v -> x = -v -> x;
    v -> y = -v -> y;   
}

f32 v2_mag(v2_t v) {
    return (sqrt(sq(v.x) + sq(v.y))); 
}

f32 v2_angle(v2_t v) {
    return atan2(v.y, v.x);
}

f32 v2_dot_prod(v2_t v1, v2_t v2) {
    return v1.x * v2.x + v1.y * v2.y; 
}

f32 v2_dist_between(v2_t v1, v2_t v2) {
    f32 dx = v1.x - v2.x;
    f32 dy = v1.y - v2.y;
    return sqrt(sq(dx) + sq(dy));
}

f32 v2_angle_between(v2_t v1, v2_t v2) {
    if (v1.x == 0 && v1.y == 0) return 0.0f;
    if (v2.x == 0 && v2.y == 0) return 0.0f;

    f32 dot = v2_dot_prod(v1, v2);
    f32 v1mag = v2_mag(v1);
    f32 v2mag = v2_mag(v2);
    f32 amt = dot / (v1mag * v2mag);

    if (amt <= -1) {
        return pi;
    } else if (amt >= 1) {
        return 0;
    }
    return acos(amt);
}

v2_t v2_projection(v2_t v1, v2_t v2) {
    f32 dot = v2_dot_prod(v1, v2);
    f32 len = v2_dot_prod(v2, v2);
    if (len == 0.0f) return v2;

    v2_t return_val = v2;
    v2_scale(&return_val, dot / len);
    return return_val;
}


v3_t v3_add(v3_t v1, v3_t v2) {
    return mk_v3(v1.x + v2.x,
                 v1.y + v2.y,
                 v1.z + v2.z);
}

v3_t v3_sub(v3_t v1, v3_t v2) {
    return mk_v3(v2.x - v1.x,
                 v2.y - v1.y,
                 v2.z - v1.z);
}

v3_t v3_lerp(v3_t v1, v3_t v2, f32 amt) {
    v1.x = ilerp(v1.x, v2.x, amt);
    v1.y = ilerp(v1.y, v2.y, amt);   
    v1.z = ilerp(v1.z, v2.z, amt);
    return v1;
}

void v3_limit(v3_t * v, f32 scalar) {
    assert(v != NULL);
    f32 len = sq(v -> x) + sq(v -> y);
    f32 len_t = sq(scalar);
    if (len > len_t) {
        v3_normalize(v);
        v3_scale(v, scalar);
    }   
}

void v3_scale(v3_t * v, f32 scalar) {
    assert(v != NULL);
    v -> x *= scalar;
    v -> y *= scalar;
    v -> z *= scalar;
}

void v3_normalize(v3_t * v) {
    assert(v != NULL);
    f32 mag = v3_mag(*v);


    if (mag != 1.0f) {
        v -> x = v -> x / mag;
        v -> y = v -> y / mag;
        v -> z = v -> z / mag;
    }
}

void v3_mag_set(v3_t * v, f32 scalar) {
    assert(v != NULL);
    v3_normalize(v);
    v3_scale(v, scalar);
}

void v3_negate(v3_t * v) {
    assert(v != NULL);
    v -> x = -v -> x;
    v -> y = -v -> y;
    v -> z = -v -> z;
}

f32 v3_mag(v3_t v) {
    return (sqrt(sq(v.x) + sq(v.y) + sq(v.z))); 
}

f32 v3_dot_prod(v3_t v1, v3_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;  
}

v3_t v3_cross_prod(v3_t v1, v3_t v2) {
    v3_t v_o;
    v_o.x = v1.y * v2.z - v1.z * v2.y;
    v_o.y = v1.z * v2.x - v1.z * v2.z;
    v_o.z = v1.x * v2.y - v1.y * v2.x;
    return v_o;
}


f32 v3_dist_between(v3_t v1, v3_t v2) {
    f32 dx = v2.x - v1.x;
    f32 dy = v2.y - v1.y;
    f32 dz = v2.z - v1.z;
    return sqrt(sq(dx) + sq(dy) + sq(dz)); 
}



f32 v3_angle_between(v3_t v1, v3_t v2) {
    if (v1.x == 0 && v1.y == 0) return 0.0f;
    if (v2.x == 0 && v2.y == 0) return 0.0f;
    if (v2.z == 0 && v2.z == 0) return 0.0f;

    f32 dot = v3_dot_prod(v1, v2);
    f32 v1mag = v3_mag(v1);
    f32 v2mag = v3_mag(v2);
    f32 amt = dot / (v1mag * v2mag);

    if (amt <= -1) {
        return pi;
    } else if (amt >= 1) {
        return 0;
    }
    return acos(amt);   
}

v3_t v3_projection(v3_t v1, v3_t v2) {
    f32 dot = v3_dot_prod(v1, v2);
    f32 len = v3_dot_prod(v2, v2);
    if (len == 0.0f) return v2;

    v3_t return_val = v2;
    v3_scale(&return_val, dot / len);
    return return_val;   
}

v4_t v4_add(v4_t v1, v4_t v2) {
    return mk_v4((v1.x + v2.x),
          (v1.y + v2.y),
          (v1.z + v2.z),
          (v1.w + v2.w));  
}

v4_t v4_sub(v4_t v1, v4_t v2) {
    return mk_v4((v2.x - v1.x),
          (v2.y - v1.y),
          (v2.z - v1.z),
          (v2.w - v1.w));    
}

v4_t v4_lerp(v4_t v1, v4_t v2, f32 amt) {
    v4_t vec;
    vec.x = ilerp(v1.x, v2.x, amt);
    vec.y = ilerp(v1.y, v2.y, amt);   
    vec.z = ilerp(v1.z, v2.z, amt);
    vec.w = ilerp(v1.w, v2.w, amt);
    return vec;
}

void v4_limit(v4_t * v, f32 scalar) {
    assert(v != NULL);
    f32 len = sq(v -> x) + sq(v -> y);
    f32 len_t = sq(scalar);
    if (len > len_t) {
        v4_normalize(v);
        v4_scale(v, scalar);
    }        
}

void v4_scale(v4_t * v, f32 scalar) {
    assert(v != NULL);
    v -> x *= scalar;
    v -> y *= scalar;  
    v -> z *= scalar;
    v -> w *= scalar;
}

void v4_normalize(v4_t * v) {
    assert(v != NULL);
    f32 mag = v4_mag(*v);
    if (mag != 1.0f) {
        v -> x = v -> x / mag;
        v -> y = v -> y / mag;
        v -> z = v -> z / mag;
        v -> w = v -> w / mag;
    }
}

void v4_mag_set(v4_t * v, f32 scalar) {
    assert(v != NULL);
    v4_normalize(v);
    v4_scale(v, scalar); 
}

void v4_negate(v4_t * v) {
    assert(v != NULL);
    v -> x = -v -> x;
    v -> y = -v -> y;
    v -> z = -v -> z;
    v -> w = -v -> w;
}


f32 v4_mag(v4_t v) {
    return (sqrt(sq(v.x) + sq(v.y) + sq(v.z) + sq(v.w))); 
}

f32 v4_dot_prod(v4_t v1, v4_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;  
}

f32 v4_dist_between(v4_t v1, v4_t v2) {
    f32 dx = v2.x - v1.x;
    f32 dy = v2.y - v1.y;
    f32 dz = v2.z - v1.z;
    f32 dw = v2.w - v1.w;
    return sqrt(sq(dx) + sq(dy) + sq(dz) + sq(dw)); 
}

// projects v1 on v2
v4_t v4_projection(v4_t v1, v4_t v2) {
    f32 dot = v4_dot_prod(v1, v2);
    f32 len = v4_dot_prod(v2, v2);
    if (len == 0.0f) return v2;

    v4_t return_val = v2;
    v4_scale(&return_val, dot / len);
    return return_val;   
}

v3_t v2_to_v3(const v2_t v) {
    return mk_v3(v.x, v.y, 1);
}

v4_t v3_to_v4(const v3_t v) {
    return mk_v4(v.x, v.y, v.z, 1);
}

v3_t v4_to_v3(const v4_t v) {
    return mk_v3(v.x, v.y, v.z);
}

v2_t v3_to_v2(const v3_t v) {
    return mk_v2(v.x, v.y);
}


m4x4_t mk_m4x4(f32 m00, f32 m01, f32 m02, f32 m03,
               f32 m04, f32 m05, f32 m06, f32 m07,
               f32 m08, f32 m09, f32 m10, f32 m11,
               f32 m12, f32 m13, f32 m14, f32 m15) { 
    m4x4_t mat = {{
        {m00, m01, m02, m03},
        {m04, m05, m06, m07},
        {m08, m09, m10, m11},
        {m12, m13, m14, m15}
    }};

    return mat;
}

m4x4_t mk_zero_m4x4() {
    return mk_m4x4( 
                    0, 0, 0, 0,
                    0, 0, 0, 0, 
                    0, 0, 0, 0, 
                    0, 0, 0, 0
                  );
}

m4x4_t mk_identity_m4x4() {
    return mk_m4x4(
                     1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1
                    
                  );
}

m4x4_t mk_diag_m4x4(f32 val) {
    return mk_m4x4(
                    val, 000, 000, 000,
                    000, val, 000, 000,
                    000, 000, val, 000,
                    000, 000, 000, val
                  );
}

m4x4_t m4x4_add(m4x4_t m1, m4x4_t m2) {
    for (u16 i = 0; i < 4; i++) {
        for (u16 j = 0; j < 4; j++) {
            m1.elems[i][j] += m2.elems[i][j];
        }
    }
    m4x4_t mat = m1;
    return mat;
}

m4x4_t m4x4_sub(m4x4_t m1, m4x4_t m2) {    
    for (u16 i = 0; i < 4; i++) {
        for (u16 j = 0; j < 4; j++) {
            m2.elems[i][j] -= m1.elems[i][j];
        }
    }
    m4x4_t mat = m2;
    return mat;
} 
m4x4_t m4x4_mult(m4x4_t m1, m4x4_t m2) {
 	m4x4_t result = mk_zero_m4x4(); 
	for (u32 y = 0; y < 4; ++y) {
		for (u32 x = 0; x < 4; ++x) {
			f32 sum = 0.0f;
			for (u32 e = 0; e < 4; ++e) {
				sum += m1.elems[e][x] * m2.elems[y][e];
			}
			result.elems[y][x] = sum;
		}
	}

	return result;
} 

m4x4_t m4x4_mult_n(u32 m4x4_count, ...) {
    va_list matrices;
    va_start(matrices, m4x4_count);
        m4x4_t ident = mk_identity_m4x4();
        for (u32 i = 0; i < m4x4_count; ++i) {
            ident = m4x4_mult(ident, va_arg(matrices, m4x4_t));
        }

    va_end(matrices);

    return ident;
}

v3_t m4x4_mult_v3(m4x4_t m, v3_t v) {
    return v4_to_v3(m4x4_mult_v4(m, mk_v4(v.x, v.y, v.z, 1)));
}

v4_t m4x4_mult_v4(m4x4_t m, v4_t v) {
    return mk_v4(
                m.elems[0][0] * v.x + m.elems[1][0] * v.y + m.elems[2][0] * v.z + m.elems[3][0] * v.w,  
                m.elems[0][1] * v.x + m.elems[1][1] * v.y + m.elems[2][1] * v.z + m.elems[3][1] * v.w,  
                m.elems[0][2] * v.x + m.elems[1][2] * v.y + m.elems[2][2] * v.z + m.elems[3][2] * v.w,  
                m.elems[0][3] * v.x + m.elems[1][3] * v.y + m.elems[2][3] * v.z + m.elems[3][3] * v.w
         );
}

m4x4_t m4x4_scale_by_v3(m4x4_t mat, const v3_t vector) {
    mat.elems[0][0] *= vector.x;
    mat.elems[1][1] *= vector.y;
    mat.elems[2][2] *= vector.z;
    return mat;
}

// Takes in an identity matrix
m4x4_t m4x4_translate_by_v3(m4x4_t mat, const v3_t vector) {
    mat.elems[3][0] += vector.x;
    mat.elems[3][1] += vector.y;
    mat.elems[3][2] += vector.z;
    return mat;
}

m4x4_t m4x4_rotate_by_v3(f32 degrees, v3_t axis) {
    m4x4_t mat = mk_identity_m4x4();

    f32 a = degrees;
    f32 c = (f32)cosf(a);
    f32 s = (f32)sinf(a);

    v3_normalize(&axis);
    f32 x = axis.x;
    f32 y = axis.y;
    f32 z = axis.z;

    //First column
    mat.elems[0][0] = c + (x * x) * (1 - c);
    mat.elems[0][1] = x * y * (1 - c) - (z * s);
    mat.elems[0][2] = x * z * (1 - c) + (y * s);

    //Second column
    mat.elems[1][0] = y * x * (1 - c) + z * s;
    mat.elems[1][1] = c + y * y * (1 - c);
    mat.elems[1][2] = y * z * (1 - c) - x * s;

    //Third column
    mat.elems[2][0] = z * x * (1 - c) - y * s;
    mat.elems[2][1] = z * y * (1 - c) + x * s;
    mat.elems[2][2] = c + z * z * (1 - c);

    return mat;

}

m4x4_t m4x4_orthographic_projection(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) {
    return mk_m4x4
    (
        
        2.0f / (r - l), 0, 0,  0,
        0, +2.0f / (t - b), 0, 0,
        0, 0, -2.0f / (f - n), 0,
        -(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f
               
    );
}

void prof_start(prof_t * profiler, char * name) {
    assert(profiler != NULL && name != NULL);
    profiler -> name = name;
    profiler -> elapsed = 0.0f;
    profiler -> start = clock();
}

void prof_end(prof_t * profiler) {
    assert(profiler != NULL);
    profiler -> end = clock();
    profiler -> elapsed = (f64)(profiler -> end - profiler -> start) / CLOCKS_PER_SEC;
}

char * i32_to_str(i32 num) {
    char * string = calloc(64, sizeof(char)); 
    assert(string != NULL);
    snprintf(string, 64, "%d", num); 
    return string;
}

char * f64_to_str(f64 num) {
    char * string = malloc(64); 
    assert(string != NULL);
    snprintf(string, 64, "%f", num); 
    return string;   
}

// From: https://github.com/TheAlgorithms/C
i32 str_to_i32(char * string) {
    int i;
    int sign;
    long value;
    long prev;

    i = 0;
    sign = 1;
    value = 0;

    while (((string[i] <= 13 && string[i] >= 9) || string[i] == 32) && string[i] != '\0')
        i++;

    if (string[i] == '-') {
        sign = -1;
	    i++;
	}

    else if (string[i] == '+') {
        sign = 1;
	    i++;
	}

    while (string[i] >= 48 && string[i] <= 57 && string[i] != '\0') {
        prev = value;
        value = value * 10 + sign * (string[i] - '0');

        if (sign == 1 && prev > value)
            return (-1);
        else if (sign == -1 && prev < value)
            return (0);
        i++;
    }
    return (value); 
}


config_file_t parse_config_file(const char * filepath) {
    config_file_t config = {0};
    // problems here?
    config.file = *txt_file_query(filepath);
    char * str = config.file.content;
    for (u64 u = 0; u < strlen(str); u++) {
        if (!isalnum(str[u]) && isalnum(str[u - 1]) && !isdigit(str[u]) && str[u] != '_') {
            config.len++;
        }
        if (str[u] == ';') {
            config.len++;
        }
    }
    u32 * word_len = calloc(config.len + 1, sizeof(u32));
    
    u32 len = 0;
    u32 w_count = 0;
    for (u64 u = 0; u < strlen(str); u++) {
        if (isalnum(str[u]) || str[u] == '_') {
            len++;
        }

        if (str[u] == ';' ) {
            len++;
            word_len[w_count] = len;
            len = 0;
            w_count++;
        }

        if (!isalnum(str[u]) && str[u] != '_') {
            word_len[w_count] = len;
            len = 0;
            if (isalnum(str[u - 1]) && str[u - 1] != '_') {
                w_count++; 
            }
        }
    }

    config.max_str_byte_size = 0;
    for (u64 u = 0; u < w_count; u++) {
        if (word_len[u] * sizeof(void*) > config.max_str_byte_size) {
            config.max_str_byte_size = word_len[u] * sizeof(void*);
        }
    }


    char ** words = calloc(config.len, sizeof(void*));
    for (u64 s = 0; s < config.len; s++) {
        // added max size in bytes as this is the biggest possible string
        words[s] = calloc(word_len[s], config.max_str_byte_size); 
    }
    u64 word_idx = 0; 
    u64 word_char_idx = 0;
    for (u64 u = 0; u < strlen(str); u++) {
        if (isalnum(str[u]) || isdigit(str[u]) || str[u] == '_') {
            words[word_idx][word_char_idx] = str[u];
            word_char_idx++;
        }  
        if ((!isalnum(str[u]) && isalnum(str[u - 1])) && !isdigit(str[u]) && str[u] != '_' && str[u] != ';') {
            if (word_idx < config.len) {
                word_idx++;
            }
            word_char_idx = 0; 
        }

        if (str[u] == ';') {
            words[word_idx][word_char_idx] = str[u];
            word_char_idx++;           
            if (word_idx < config.len) {
                word_idx++;
            }
            word_char_idx = 0; 
        }
    }

    config.data = words;

    u32 prev = 0;
    config.max_elems_in_list = 0;
    for (u32 u = 0; u < len; u++) {
        if (config.data[u][0] != ';') {
            config.max_elems_in_list++;
        }
        else {
            if (config.max_elems_in_list > prev) {
                prev = config.max_elems_in_list;
                config.max_elems_in_list = 0;
            }
        }
    }
    config.max_elems_in_list = prev;
    
    if (config.len % 2 == 0) {
        config.table = ht_create(config.len);
    } else {
        config.table = ht_create(config.len + 1);
    }

    char ** data = config.data;
    len = config.len;
    ht_t * table = &config.table;
    u32 c_len = config.max_str_byte_size;
    u32 max_elems = config.max_elems_in_list * config.len;

    void ** tmp = NULL;
    void ** target = NULL;

    // used to be void*
    tmp = calloc(max_elems + 1, c_len);
    for (u32 u = 0; u < len; u++) {
        char * key = data[u];
        u32 d_id = u + 1;
        u32 curr_inp = 0;
        while (data[d_id][0] != ';') {
            tmp[curr_inp] = data[d_id];
            curr_inp++;
            d_id++;
        }
        target = calloc(curr_inp + 2, c_len);
        target[0] = (void*) curr_inp + 1;
        for (u32 n = 1; n < curr_inp + 1; n++) {
            target[n] = (void*) tmp[n - 1];
        }

        ht_insert(table, key, target);
        u += d_id - u;
    }

    return config; 
}

void ** config_file_get(config_file_t * file, char * key) {
    return ht_get(&file -> table, key);
}