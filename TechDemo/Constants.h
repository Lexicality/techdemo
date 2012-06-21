//
// TechDemo/Constants.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once


// Messy but whateverrrr
#define CREATE_EXCEPTION(NAME, DERIVED) class NAME : public DERIVED { public: NAME(const std::string& what) : DERIVED(what) {} }

// hurf.
typedef unsigned short int word;
typedef unsigned char      byte;
