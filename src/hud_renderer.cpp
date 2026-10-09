#include "hud_renderer.h"

#include <glad/glad.h>
#include <array>
#include <cctype>

using namespace std;

namespace
{
array<unsigned char,7> glyph(char ch)
{
    switch(ch)
    {
    case 'A':return {14,17,17,31,17,17,17};case 'B':return {30,17,17,30,17,17,30};
    case 'C':return {14,17,16,16,16,17,14};case 'D':return {30,17,17,17,17,17,30};
    case 'E':return {31,16,16,30,16,16,31};case 'F':return {31,16,16,30,16,16,16};
    case 'G':return {14,17,16,23,17,17,15};case 'H':return {17,17,17,31,17,17,17};
    case 'I':return {14,4,4,4,4,4,14};case 'J':return {7,2,2,2,18,18,12};
    case 'K':return {17,18,20,24,20,18,17};case 'L':return {16,16,16,16,16,16,31};
    case 'M':return {17,27,21,21,17,17,17};case 'N':return {17,25,21,19,17,17,17};
    case 'O':return {14,17,17,17,17,17,14};case 'P':return {30,17,17,30,16,16,16};
    case 'Q':return {14,17,17,17,21,18,13};case 'R':return {30,17,17,30,20,18,17};
    case 'S':return {15,16,16,14,1,1,30};case 'T':return {31,4,4,4,4,4,4};
    case 'U':return {17,17,17,17,17,17,14};case 'V':return {17,17,17,17,17,10,4};
    case 'W':return {17,17,17,21,21,21,10};case 'X':return {17,17,10,4,10,17,17};
    case 'Y':return {17,17,10,4,4,4,4};case 'Z':return {31,1,2,4,8,16,31};
    case '0':return {14,17,19,21,25,17,14};case '1':return {4,12,4,4,4,4,14};
    case '2':return {14,17,1,2,4,8,31};case '3':return {30,1,1,14,1,1,30};
    case '4':return {2,6,10,18,31,2,2};case '5':return {31,16,16,30,1,1,30};
    case '6':return {14,16,16,30,17,17,14};case '7':return {31,1,2,4,8,8,8};
    case '8':return {14,17,17,14,17,17,14};case '9':return {14,17,17,15,1,1,14};
    case '.':return {0,0,0,0,0,12,12};case ':':return {0,12,12,0,12,12,0};
    case '-':return {0,0,0,31,0,0,0};case '+':return {0,4,4,31,4,4,0};
    case '/':return {1,1,2,4,8,16,16};case '%':return {17,2,4,8,17,0,0};
    default:return {};
    }
}
}

HudRenderer::HudRenderer():shader_("shaders/simulation.vs","shaders/simulation.fs")
{
    glGenVertexArrays(1,&vao_);glGenBuffers(1,&vbo_);
    glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,6*sizeof(float),nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,6*sizeof(float),
                          reinterpret_cast<void*>(2*sizeof(float)));
    glEnableVertexAttribArray(1);glBindVertexArray(0);
}

HudRenderer::~HudRenderer()
{
    glDeleteBuffers(1,&vbo_);glDeleteVertexArrays(1,&vao_);
}

void HudRenderer::begin(){vertices_.clear();}

void HudRenderer::rectangle(float x,float y,float width,float height,
                            const glm::vec4& color)
{
    const float points[]={x,y,x+width,y,x+width,y+height,x,y,
                          x+width,y+height,x,y+height};
    for(int index=0;index<12;index+=2)
        vertices_.insert(vertices_.end(),{points[index],points[index+1],
            color.r,color.g,color.b,color.a});
}

void HudRenderer::text(const string& value,float x,float y,float pixel,
                       const glm::vec4& color)
{
    for(unsigned char ch:value)
    {
        const auto rows=glyph(static_cast<char>(toupper(ch)));
        for(int row=0;row<7;++row)for(int column=0;column<5;++column)
            if(rows[row]&(1<<(4-column)))
                rectangle(x+column*pixel,y+row*pixel,pixel,pixel,color);
        x+=6*pixel;
    }
}

void HudRenderer::flush(int width,int height)
{
    if(vertices_.empty()||width<=0||height<=0)return;
    shader_.use();shader_.setVec2("resolution",glm::vec2(width,height));
    glDisable(GL_DEPTH_TEST);glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,vertices_.size()*sizeof(float),vertices_.data(),GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES,0,static_cast<int>(vertices_.size()/6));
    glBindVertexArray(0);glDisable(GL_BLEND);glEnable(GL_DEPTH_TEST);
}
