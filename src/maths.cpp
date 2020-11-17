#include <cmath>
#include <cassert>

#include "maths.h"

const float epsilon = 0.0001f;

/*
 * I have generated my math code using simple embedded python scripts. 
 * The system I am using to execute these scripts is called COG.
 *
 * Please see here for details: https://nedbatchelder.com/code/cog/index.html
 */

/*[[[cog

for i in range(2, 5):
    cog.outl(f"""
///VECTOR v{i} IMPLEMENTATION
float & v{i}::operator [] (const int idx){{ return e[idx]; }}

const float & v{i}::operator [] (const int idx) const{{ return e[idx]; }}

bool v{i}::operator == (const v{i} & rhs){{
    for (auto i = 0; i < {i}; i++) {{
        const auto dif = abs(e[i] - rhs[i]);
        if(dif > epsilon) return false;
    }}
    return true;
}}

v{i} v{i}::operator - (const  v{i} & rhs) const {{
    v{i} res{{}};
    for (auto i = 0; i < {i}; i++) res[i] = e[i] - rhs[i];
    return res;
}}

v{i} v{i}::operator - (const float rhs) const {{
    v{i} res{{}};
    for (auto i = 0; i < {i}; i++) res[i] = e[i] - rhs;
    return res;
}}

v{i} v{i}::operator / (const float rhs) const
{{
    v{i} res{{}};
    for (auto i = 0; i < {i}; i++) res[i] = e[i] / rhs;
    return res;
}}


v{i} v{i}::operator + (const v{i} & rhs){{
    v{i} res{{}};
    for (auto i = 0; i < {i}; i++) res[i] = e[i] + rhs[i];
    return res;
}}

v{i} v{i}::operator + (const float rhs){{
    v{i} res{{}};
    for (auto i = 0; i < {i}; i++) res[i] = e[i] + rhs;
    return res;
}}

float v{i}::operator * (const v{i} & rhs){{
    float res = 0;
    for (auto i = 0; i < {i}; i++) res += e[i] * rhs[i];
    return res;
}}

v{i} v{i}::operator * (const float rhs){{
    v{i} res{{}};
    for (auto i = 0; i < {i}; i++) res[i] = e[i] * rhs;
    return res;
}}

float v{i}::inner(const v{i} & rhs){{
    float res = 0;
    for (auto i = 0; i <{i}; i++) res += e[i] * rhs[i];
    return res;
}}

inline float v{i}::length_sq(){{
    return inner(*this);
}}

inline float v{i}::length(){{
    return sqrt(length_sq());
}}

v{i} v{i}::normalise(){{
    v{i} res{{}};

    const auto len = length();
    for (auto i = 0; i < {i}; i++){{
        res[i] = e[i] / len;
    }}

    return res;
}}
""")
]]]*/

///VECTOR v2 IMPLEMENTATION
float & v2::operator [] (const int idx){ return e[idx]; }

const float & v2::operator [] (const int idx) const{ return e[idx]; }

bool v2::operator == (const v2 & rhs){
    for (auto i = 0; i < 2; i++) {
        const auto dif = abs(e[i] - rhs[i]);
        if(dif > epsilon) return false;
    }
    return true;
}

v2 v2::operator - (const  v2 & rhs) const {
    v2 res{};
    for (auto i = 0; i < 2; i++) res[i] = e[i] - rhs[i];
    return res;
}

v2 v2::operator - (const float rhs) const {
    v2 res{};
    for (auto i = 0; i < 2; i++) res[i] = e[i] - rhs;
    return res;
}

v2 v2::operator / (const float rhs) const
{
    v2 res{};
    for (auto i = 0; i < 2; i++) res[i] = e[i] / rhs;
    return res;
}


v2 v2::operator + (const v2 & rhs){
    v2 res{};
    for (auto i = 0; i < 2; i++) res[i] = e[i] + rhs[i];
    return res;
}

v2 v2::operator + (const float rhs){
    v2 res{};
    for (auto i = 0; i < 2; i++) res[i] = e[i] + rhs;
    return res;
}

float v2::operator * (const v2 & rhs){
    float res = 0;
    for (auto i = 0; i < 2; i++) res += e[i] * rhs[i];
    return res;
}

v2 v2::operator * (const float rhs){
    v2 res{};
    for (auto i = 0; i < 2; i++) res[i] = e[i] * rhs;
    return res;
}

float v2::inner(const v2 & rhs){
    float res = 0;
    for (auto i = 0; i <2; i++) res += e[i] * rhs[i];
    return res;
}

inline float v2::length_sq(){
    return inner(*this);
}

inline float v2::length(){
    return sqrt(length_sq());
}

v2 v2::normalise(){
    v2 res{};

    const auto len = length();
    for (auto i = 0; i < 2; i++){
        res[i] = e[i] / len;
    }

    return res;
}


///VECTOR v3 IMPLEMENTATION
float & v3::operator [] (const int idx){ return e[idx]; }

const float & v3::operator [] (const int idx) const{ return e[idx]; }

bool v3::operator == (const v3 & rhs){
    for (auto i = 0; i < 3; i++) {
        const auto dif = abs(e[i] - rhs[i]);
        if(dif > epsilon) return false;
    }
    return true;
}

v3 v3::operator - (const  v3 & rhs) const {
    v3 res{};
    for (auto i = 0; i < 3; i++) res[i] = e[i] - rhs[i];
    return res;
}

v3 v3::operator - (const float rhs) const {
    v3 res{};
    for (auto i = 0; i < 3; i++) res[i] = e[i] - rhs;
    return res;
}

v3 v3::operator / (const float rhs) const
{
    v3 res{};
    for (auto i = 0; i < 3; i++) res[i] = e[i] / rhs;
    return res;
}


v3 v3::operator + (const v3 & rhs){
    v3 res{};
    for (auto i = 0; i < 3; i++) res[i] = e[i] + rhs[i];
    return res;
}

v3 v3::operator + (const float rhs){
    v3 res{};
    for (auto i = 0; i < 3; i++) res[i] = e[i] + rhs;
    return res;
}

float v3::operator * (const v3 & rhs){
    float res = 0;
    for (auto i = 0; i < 3; i++) res += e[i] * rhs[i];
    return res;
}

v3 v3::operator * (const float rhs){
    v3 res{};
    for (auto i = 0; i < 3; i++) res[i] = e[i] * rhs;
    return res;
}

float v3::inner(const v3 & rhs){
    float res = 0;
    for (auto i = 0; i <3; i++) res += e[i] * rhs[i];
    return res;
}

inline float v3::length_sq(){
    return inner(*this);
}

inline float v3::length(){
    return sqrt(length_sq());
}

v3 v3::normalise(){
    v3 res{};

    const auto len = length();
    for (auto i = 0; i < 3; i++){
        res[i] = e[i] / len;
    }

    return res;
}


///VECTOR v4 IMPLEMENTATION
float & v4::operator [] (const int idx){ return e[idx]; }

const float & v4::operator [] (const int idx) const{ return e[idx]; }

bool v4::operator == (const v4 & rhs){
    for (auto i = 0; i < 4; i++) {
        const auto dif = abs(e[i] - rhs[i]);
        if(dif > epsilon) return false;
    }
    return true;
}

v4 v4::operator - (const  v4 & rhs) const {
    v4 res{};
    for (auto i = 0; i < 4; i++) res[i] = e[i] - rhs[i];
    return res;
}

v4 v4::operator - (const float rhs) const {
    v4 res{};
    for (auto i = 0; i < 4; i++) res[i] = e[i] - rhs;
    return res;
}

v4 v4::operator / (const float rhs) const
{
    v4 res{};
    for (auto i = 0; i < 4; i++) res[i] = e[i] / rhs;
    return res;
}


v4 v4::operator + (const v4 & rhs){
    v4 res{};
    for (auto i = 0; i < 4; i++) res[i] = e[i] + rhs[i];
    return res;
}

v4 v4::operator + (const float rhs){
    v4 res{};
    for (auto i = 0; i < 4; i++) res[i] = e[i] + rhs;
    return res;
}

float v4::operator * (const v4 & rhs){
    float res = 0;
    for (auto i = 0; i < 4; i++) res += e[i] * rhs[i];
    return res;
}

v4 v4::operator * (const float rhs){
    v4 res{};
    for (auto i = 0; i < 4; i++) res[i] = e[i] * rhs;
    return res;
}

float v4::inner(const v4 & rhs){
    float res = 0;
    for (auto i = 0; i <4; i++) res += e[i] * rhs[i];
    return res;
}

inline float v4::length_sq(){
    return inner(*this);
}

inline float v4::length(){
    return sqrt(length_sq());
}

v4 v4::normalise(){
    v4 res{};

    const auto len = length();
    for (auto i = 0; i < 4; i++){
        res[i] = e[i] / len;
    }

    return res;
}

/*[[[end]]]*/

inline v3 cross(const v3 &v1, const v3 &v2) {
    return v3{v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}

inline v4 project_4d(const v3& v, float end) {
    return { v.x, v.y, v.z, end };
}

inline v3 project_3d(const v4& v) {
    assert(v.w != 0);
    return { v.x / v.w, v.y / v.w, v.z / v.w };
}

v2_i v3_to_v2(const v3& v)
{
    return { static_cast<int>(roundf(v.x)), static_cast<int>(roundf(v.y))};
}

v2_i operator + (const v2_i& lhs, const v2_i& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}

v2_i operator - (const v2_i& lhs, const v2_i& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}

/*[[[cog
def invert(i):
    if (i > 2):
        return f"""
m{i} m{i}::invert() const{{
    const auto det = determinate();
    assert(det != 0);

    const auto adj = adjugate();

    const auto inverse_det = 1.0f / det;

    const auto ret = adj * inverse_det;

    return ret;
}}
    """
    return ""

def adjugate(i):
    if(i > 2):
        return f"""
m{i} m{i}::adjugate() const{{
    m{i} ret{{}};

    for(auto row = 0; row < {i}; row++){{
        for(auto col = 0; col < {i}; col++){{
            ret[row][col] = powf(-1.0f, static_cast<float>(row + col)) * get_sub_mat(row, col).determinate();
        }}
    }}

    return ret.transpose();
}}
"""
    return ""

def submat(i):
    if (i > 2):
        return  f"""
inline m{i - 1} m{i}::get_sub_mat(int row_to_skip, int col_to_skip) const{{
    m{i - 1} ret{{}};

    for(auto i_row = 0; i_row < {i}; i_row++){{
        if(i_row == row_to_skip) continue;

        for (auto i_col = 0; i_col < {i}; i_col++){{
            if(i_col == col_to_skip) continue;
            
            ret.e[i_row > row_to_skip ? i_row - 1 : i_row]
                                [i_col > col_to_skip ? i_col - 1 : i_col] = e[i_row][i_col];
        }}
    }}

    return ret;
}}"""
    
    return ""

def determinate(i):
    if (i > 2):
        return f"""
inline float m{i}::determinate() const{{
    float det = 0;

    for (auto i = 0; i < {i}; i++){{
        auto sample = e[0][i];    
        
        if(i % 2 != 0) sample *=  -1;

        auto sub_mat = get_sub_mat(0, i);
        const auto sub_mat_det = sub_mat.determinate();
        
        det += sample * sub_mat_det;
    }}

    return det;
}}"""
    else:
        return f"""
inline float m{i}::determinate() const{{
    return r1.x * r2.y - r1.y * r2.x;
}}"""


for i in range(2, 5):
    cog.outl(f"""
inline v{i} & m{i}::operator[](const int idx){{ return e[idx]; }}
inline const v{i} & m{i}::operator[](const int idx) const{{ return e[idx]; }}

inline m{i} m{i}::operator * (const m{i} & rhs) const{{
    m{i} ret{{}};
    
    for (auto row = 0; row < {i}; row++){{
        for (auto col = 0; col < {i}; col++){{
            float sum = 0;
            for (auto i = 0; i < {i}; i++){{
                sum += e[row][i] * rhs[i][col];                
            }}
            ret[row][col]= sum;

        }}
    }}

    return ret;
}}

inline v{i} m{i}::operator *(const v{i} & rhs) const{{
    v{i} res{{}};
    for (auto row = 0; row < {i}; row++){{
        for (auto col = 0; col < {i}; col++){{
            res[row] += e[row][col] * rhs[col];
        }}
    }}
    
    return res;
}}

inline m{i} m{i}::operator * (const float rhs) const{{
    m{i} ret{{}};
    
    for (auto row = 0; row < {i}; row++){{
        for (auto col = 0; col < {i}; col++){{
            ret.e[row][col] = e[row][col] * rhs;
        }}
    }}

    return ret;
}}

inline bool m{i}::operator == (const m{i} & rhs) const{{
    for(auto row = 0; row < {i}; row++){{
        for(auto col = 0; col < {i}; col++){{
            const auto diff = abs(e[row][col] - rhs[row][col]);
            
            if(diff > epsilon){{
                return false;
            }}
        }}
    }}

    return true;
}}

{submat(i)}

{determinate(i)}

{adjugate(i)}

{invert(i)}

inline m{i}  m{i}::transpose() const{{
    m{i} ret{{}};

    for (auto row = 0; row < {i}; row++){{
        for(auto col = 0; col < {i}; col++){{
            ret[row][col] = e[col][row];
        }}
    }}

    return ret;
}}

inline float  m{i}::sum() const{{
    float ret = 0;

    for (auto row = 0; row < {i}; row++){{
        for(auto col = 0; col < {i}; col++){{
            ret += e[row][col];
        }}
    }}

    return ret;
}}
""")
]]]*/

inline v2 & m2::operator[](const int idx){ return e[idx]; }
inline const v2 & m2::operator[](const int idx) const{ return e[idx]; }

inline m2 m2::operator * (const m2 & rhs) const{
    m2 ret{};
    
    for (auto row = 0; row < 2; row++){
        for (auto col = 0; col < 2; col++){
            float sum = 0;
            for (auto i = 0; i < 2; i++){
                sum += e[row][i] * rhs[i][col];                
            }
            ret[row][col]= sum;

        }
    }

    return ret;
}

inline v2 m2::operator *(const v2 & rhs) const{
    v2 res{};
    for (auto row = 0; row < 2; row++){
        for (auto col = 0; col < 2; col++){
            res[row] += e[row][col] * rhs[col];
        }
    }
    
    return res;
}

inline m2 m2::operator * (const float rhs) const{
    m2 ret{};
    
    for (auto row = 0; row < 2; row++){
        for (auto col = 0; col < 2; col++){
            ret.e[row][col] = e[row][col] * rhs;
        }
    }

    return ret;
}

inline bool m2::operator == (const m2 & rhs) const{
    for(auto row = 0; row < 2; row++){
        for(auto col = 0; col < 2; col++){
            const auto diff = abs(e[row][col] - rhs[row][col]);
            
            if(diff > epsilon){
                return false;
            }
        }
    }

    return true;
}




inline float m2::determinate() const{
    return r1.x * r2.y - r1.y * r2.x;
}





inline m2  m2::transpose() const{
    m2 ret{};

    for (auto row = 0; row < 2; row++){
        for(auto col = 0; col < 2; col++){
            ret[row][col] = e[col][row];
        }
    }

    return ret;
}

inline float  m2::sum() const{
    float ret = 0;

    for (auto row = 0; row < 2; row++){
        for(auto col = 0; col < 2; col++){
            ret += e[row][col];
        }
    }

    return ret;
}


inline v3 & m3::operator[](const int idx){ return e[idx]; }
inline const v3 & m3::operator[](const int idx) const{ return e[idx]; }

inline m3 m3::operator * (const m3 & rhs) const{
    m3 ret{};
    
    for (auto row = 0; row < 3; row++){
        for (auto col = 0; col < 3; col++){
            float sum = 0;
            for (auto i = 0; i < 3; i++){
                sum += e[row][i] * rhs[i][col];                
            }
            ret[row][col]= sum;

        }
    }

    return ret;
}

inline v3 m3::operator *(const v3 & rhs) const{
    v3 res{};
    for (auto row = 0; row < 3; row++){
        for (auto col = 0; col < 3; col++){
            res[row] += e[row][col] * rhs[col];
        }
    }
    
    return res;
}

inline m3 m3::operator * (const float rhs) const{
    m3 ret{};
    
    for (auto row = 0; row < 3; row++){
        for (auto col = 0; col < 3; col++){
            ret.e[row][col] = e[row][col] * rhs;
        }
    }

    return ret;
}

inline bool m3::operator == (const m3 & rhs) const{
    for(auto row = 0; row < 3; row++){
        for(auto col = 0; col < 3; col++){
            const auto diff = abs(e[row][col] - rhs[row][col]);
            
            if(diff > epsilon){
                return false;
            }
        }
    }

    return true;
}


inline m2 m3::get_sub_mat(int row_to_skip, int col_to_skip) const{
    m2 ret{};

    for(auto i_row = 0; i_row < 3; i_row++){
        if(i_row == row_to_skip) continue;

        for (auto i_col = 0; i_col < 3; i_col++){
            if(i_col == col_to_skip) continue;
            
            ret.e[i_row > row_to_skip ? i_row - 1 : i_row]
                                [i_col > col_to_skip ? i_col - 1 : i_col] = e[i_row][i_col];
        }
    }

    return ret;
}


inline float m3::determinate() const{
    float det = 0;

    for (auto i = 0; i < 3; i++){
        auto sample = e[0][i];    
        
        if(i % 2 != 0) sample *=  -1;

        auto sub_mat = get_sub_mat(0, i);
        const auto sub_mat_det = sub_mat.determinate();
        
        det += sample * sub_mat_det;
    }

    return det;
}


m3 m3::adjugate() const{
    m3 ret{};

    for(auto row = 0; row < 3; row++){
        for(auto col = 0; col < 3; col++){
            ret[row][col] = powf(-1.0f, static_cast<float>(row + col)) * get_sub_mat(row, col).determinate();
        }
    }

    return ret.transpose();
}



m3 m3::invert() const{
    const auto det = determinate();
    assert(det != 0);

    const auto adj = adjugate();

    const auto inverse_det = 1.0f / det;

    const auto ret = adj * inverse_det;

    return ret;
}
    

inline m3  m3::transpose() const{
    m3 ret{};

    for (auto row = 0; row < 3; row++){
        for(auto col = 0; col < 3; col++){
            ret[row][col] = e[col][row];
        }
    }

    return ret;
}

inline float  m3::sum() const{
    float ret = 0;

    for (auto row = 0; row < 3; row++){
        for(auto col = 0; col < 3; col++){
            ret += e[row][col];
        }
    }

    return ret;
}


inline v4 & m4::operator[](const int idx){ return e[idx]; }
inline const v4 & m4::operator[](const int idx) const{ return e[idx]; }

inline m4 m4::operator * (const m4 & rhs) const{
    m4 ret{};
    
    for (auto row = 0; row < 4; row++){
        for (auto col = 0; col < 4; col++){
            float sum = 0;
            for (auto i = 0; i < 4; i++){
                sum += e[row][i] * rhs[i][col];                
            }
            ret[row][col]= sum;

        }
    }

    return ret;
}

inline v4 m4::operator *(const v4 & rhs) const{
    v4 res{};
    for (auto row = 0; row < 4; row++){
        for (auto col = 0; col < 4; col++){
            res[row] += e[row][col] * rhs[col];
        }
    }
    
    return res;
}

inline m4 m4::operator * (const float rhs) const{
    m4 ret{};
    
    for (auto row = 0; row < 4; row++){
        for (auto col = 0; col < 4; col++){
            ret.e[row][col] = e[row][col] * rhs;
        }
    }

    return ret;
}

inline bool m4::operator == (const m4 & rhs) const{
    for(auto row = 0; row < 4; row++){
        for(auto col = 0; col < 4; col++){
            const auto diff = abs(e[row][col] - rhs[row][col]);
            
            if(diff > epsilon){
                return false;
            }
        }
    }

    return true;
}


inline m3 m4::get_sub_mat(int row_to_skip, int col_to_skip) const{
    m3 ret{};

    for(auto i_row = 0; i_row < 4; i_row++){
        if(i_row == row_to_skip) continue;

        for (auto i_col = 0; i_col < 4; i_col++){
            if(i_col == col_to_skip) continue;
            
            ret.e[i_row > row_to_skip ? i_row - 1 : i_row]
                                [i_col > col_to_skip ? i_col - 1 : i_col] = e[i_row][i_col];
        }
    }

    return ret;
}


inline float m4::determinate() const{
    float det = 0;

    for (auto i = 0; i < 4; i++){
        auto sample = e[0][i];    
        
        if(i % 2 != 0) sample *=  -1;

        auto sub_mat = get_sub_mat(0, i);
        const auto sub_mat_det = sub_mat.determinate();
        
        det += sample * sub_mat_det;
    }

    return det;
}


m4 m4::adjugate() const{
    m4 ret{};

    for(auto row = 0; row < 4; row++){
        for(auto col = 0; col < 4; col++){
            ret[row][col] = powf(-1.0f, static_cast<float>(row + col)) * get_sub_mat(row, col).determinate();
        }
    }

    return ret.transpose();
}



m4 m4::invert() const{
    const auto det = determinate();
    assert(det != 0);

    const auto adj = adjugate();

    const auto inverse_det = 1.0f / det;

    const auto ret = adj * inverse_det;

    return ret;
}
    

inline m4  m4::transpose() const{
    m4 ret{};

    for (auto row = 0; row < 4; row++){
        for(auto col = 0; col < 4; col++){
            ret[row][col] = e[col][row];
        }
    }

    return ret;
}

inline float  m4::sum() const{
    float ret = 0;

    for (auto row = 0; row < 4; row++){
        for(auto col = 0; col < 4; col++){
            ret += e[row][col];
        }
    }

    return ret;
}

/*[[[end]]]*/


m4 look_at(const v3 & eye, const v3 & center, const v3 & up){
	const auto z = (eye - center).normalise();
	const auto x = (cross(up, z)).normalise();
	const auto y = (cross(z, x)).normalise();

    return {
        x.x, x.y, x.z, -center.x,
        y.x, y.y, y.z, -center.y,
        z.x, z.y, z.z, -center.z,
        0,   0,   0,   1
    };
}

m4 identity(){
    return{
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
}

m4 view_port(const float x, const float y, const float width, const float height){
	return {
        width / 2.0f,      0.0f,              0.0f,              x + width / 2.0f,
        0.0f,              height / 2.0f,     0.0f,              y + height / 2.0f,
        0.0f,              0.0f,              255.0 / 2.0f,   255.0 / 2.0f,
        0.0f,              0.0f,              0.0f,              1.0f
    };    
}

m3 m4_to_m3(const m4 & rhs){
    return {
        rhs.r1.x, rhs.r1.y, rhs.r1.z,
        rhs.r2.x, rhs.r2.y, rhs.r2.z,
        rhs.r3.x, rhs.r3.y, rhs.r3.z 
    };
}

m4 rot_x(const float deg){
	const float theta = static_cast<float>(M_PI) / 180.0f * deg;

    return{
        1, 0, 0, 0,
        0, cos(theta), -sin(theta), 0,
        0, sin(theta), cos(theta), 0,
        0, 0, 0, 1
    };
}

m4 rot_y(const float deg){
	const float theta = static_cast<float>(M_PI) / 180.0f * deg;

    return{
        cos(theta),     0,   sin(theta), 0,
        0,              1,   0,          0,
        -sin(theta),    0,   cos(theta), 0,
        0,              0,   0,          1
    };
}

m4 projection(const v3 & eye, const v3 & center){
	const auto c = (eye - center).length();
    assert(c != 0);

    return {
        v4{1.0f, 0.0f, 0.0f,         0.0f},
        v4{0.0f, 1.0f, 0.0f,         0.0f},
        v4{0.0f, 0.0f, 1.0f,         0.0f},
        v4{0.0f, 0.0f, -1.0f / c,  1.0f}
    };
}

m4 trans(const v3 & vec){
    return {
        1,0,0,vec.x,
        0,1,0,vec.y,
        0,0,1,vec.z,
        0,0,0,1
    };
}

m4 scale(const v3& vec)
{
	auto ret = identity();
    ret.r1.x = vec.x;
    ret.r2.y = vec.y;
    ret.r3.z = vec.z;

    return ret;
}

inline int sign(const float num)
{
    return (num > 0) - (num < 0);
}