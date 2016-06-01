//
// Created by Paul A. Place on 12/16/15.
//

#ifndef BASHO_BIGSET_ACC_H
#define BASHO_BIGSET_ACC_H

#include <list>

#include <leveldb/env.h>
#include "BigsetClock.h"

namespace basho {
namespace bigset {

class BigsetAccumulator
{
    leveldb::Logger* m_pLogger;
    Actor            m_ThisActor;
    Buffer           m_CurrentSetName;
    Buffer           m_CurrentElement;
    BigsetClock      m_SetTombstone;
    DotList          m_CurrentDots;

    Buffer           m_ReadyKey;
    Buffer           m_ReadyValue;
    bool             m_ElementReady;
    bool             m_ActorClockReady;
    bool             m_ActorClockSeen;
    bool             m_ActorSetTombstoneSeen;
    bool             m_ErlangBinaryFormat; // true => the entire blob we return is in erlang's external term format; else it's in the traditional list of length-prefixed KV pairs

    void FinalizeElement();

public:
    BigsetAccumulator( const Actor& ThisActor, leveldb::Logger* pLogger )
        : m_pLogger( pLogger ),
          m_ThisActor( ThisActor ), m_ElementReady( false ),
          m_ActorClockReady( false ), m_ActorClockSeen( false ),
          m_ActorSetTombstoneSeen( false ), m_ErlangBinaryFormat( false ) // TODO: make this a parameter?
    { }

    ~BigsetAccumulator() { }

    // adds the current record to the accumulator
    //
    // NOTE: throws a std::runtime_error if an error occurs
    bool // true => record processed; false => encountered an element record but have not seen a clock for the specified actor
    AddRecord( Slice key, Slice value );

    bool
    RecordReady() const
    {
        return m_ElementReady || m_ActorClockReady;
    }

    bool
    UseErlangBinaryFormat() const
    {
        return m_ErlangBinaryFormat;
    }

    void
    GetCurrentElement( Slice& key, Slice& value );
};

} // namespace bigset
} // namespace basho

#endif // BASHO_BIGSET_ACC_H