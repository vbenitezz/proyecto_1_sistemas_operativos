#ifndef DUAL_STREAMBUF_H
#define DUAL_STREAMBUF_H

#include <iostream>
#include <fstream>

using namespace std;

class dual_streambuf : public streambuf {
    streambuf* original;
    streambuf* file;

public:
    dual_streambuf(streambuf* orig, streambuf* filebuf) : original(orig), file(filebuf) {}

protected:
    virtual int overflow(int c) override {
        if (c == EOF) return !EOF;
        if (original) original->sputc(c);
        if (file) file->sputc(c);
        return c;
    }

    virtual int sync() override {
        if (original) original->pubsync();
        if (file) file->pubsync();
        return 0;
    }
};

#endif
