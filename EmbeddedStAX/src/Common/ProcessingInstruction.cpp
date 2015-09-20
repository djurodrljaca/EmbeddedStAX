/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#include <EmbeddedStAX/Common/ProcessingInstruction.h>
#include <EmbeddedStAX/XmlValidator/ProcessingInstruction.h>

using namespace EmbeddedStAX::Common;

/**
 * Constructor
 *
 * \param piTarget  Processing instruction name
 * \param piData    Processing instruction data
 */
ProcessingInstruction::ProcessingInstruction(const std::string &piTarget,
                                             const std::string &piData)
    : m_piTarget(piTarget),
      m_piData(piData)
{
}

/**
 * Check if processing instruction is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool ProcessingInstruction::isValid() const
{
    bool valid = XmlValidator::validatePiTarget(m_piTarget);

    if (valid)
    {
        valid = XmlValidator::validatePiData(m_piData);
    }

    return valid;
}

/**
 * Clear
 */
void ProcessingInstruction::clear()
{
    m_piTarget.clear();
    m_piData.clear();
}

/**
 * Get processing instruction name ("PITarget")
 *
 * \return Processing instruction name
 */
std::string ProcessingInstruction::piTarget() const
{
    return m_piTarget;
}

/**
 * Set processing instruction name
 *
 * \param piTarget  Processing instruction name
 */
void ProcessingInstruction::setPiTarget(const std::string &piTarget)
{
    m_piTarget = piTarget;
}

/**
 * Get processing instruction data
 *
 * \return Processing instruction data
 */
std::string ProcessingInstruction::piData() const
{
    return m_piData;
}

/**
 * Set processing instruction data
 *
 * \param piData    Processing instruction data
 */
void ProcessingInstruction::setPiData(const std::string &piData)
{
    m_piData = piData;
}
