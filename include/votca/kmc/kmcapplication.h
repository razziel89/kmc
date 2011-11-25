/*
 * Copyright 2009-2011 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __VOTCA_KMC_APPLICATION_H
#define	__VOTCA_KMC_APPLICATION_H

#include <votca/tools/application.h>
#include <votca/kmc/kmccalculator.h>

namespace votca { namespace kmc {

class KMCApplication : public Application
{
public:
    KMCApplication();
   ~KMCApplication();

    void Initialize();
    /// 
    void ShowHelpText(std::ostream &out);
    /// print options from the XML file
    bool EvaluateOptions();
    /// print the description of a specific calculator
    void PrintDescription(const char *name, const bool length);
    /// add a calculator to the list
    void AddCalculator(KMCCalculator* calculator);

private:
    static const bool _short = true;
    static const bool _long = false;

    string _fwstring(string original, size_t charCount ) {
        original.resize( charCount, ' ' );
        return original;
    }

    /// List of calculators
    list<KMCCalculator *> _calculators;


};

}}

#endif	/* __VOTCA_KMC_APPLICATION_H */