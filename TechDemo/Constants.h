//
//  Constants.h
//  TechDemo
//
//  Created by Lexi Robinson on 27/05/2012.
//  Copyright (c) 2012 Lexi Robinson. All rights reserved.
//
#pragma once


// Messy but whateverrrr
#define CREATE_EXCEPTION(NAME, DERIVED) class NAME : public DERIVED { public: NAME(const std::string& what) : DERIVED(what) {} }

// hurf.
typedef unsigned short int word;
typedef unsigned char      byte;