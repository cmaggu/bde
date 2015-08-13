// ball_loggermanagerconfiguration.h                                  -*-C++-*-
#ifndef INCLUDED_BALL_LOGGERMANAGERCONFIGURATION
#define INCLUDED_BALL_LOGGERMANAGERCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a constrained-attribute class for the logger manager.
//
//@CLASSES:
// ball::LoggerManagerConfiguration: configuration spec for a logger manager
//
//@SEE_ALSO: ball_loggermanagerdefaults,
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION:  This component provides a constrained-attribute class that
// contains a set of attributes (objects and parameters) of particular use to
// logger managers.  The constraints are actively maintained by the class.  In
// particular, the "set" methods for constrained values will fail if their
// arguments are not consistent with the constraints.  Also, the constructor
// does not take any constrained arguments, but rather sets those values to
// valid defaults unconditionally.  This behavior avoids "silent failures",
// since the constructor cannot explicitly return a status value.
//
// The attributes of a 'ball::LoggerManagerConfiguration' object and their
// constraints are given, respectively, in two tables below.  Before listing
// the attributes, however, we list the types that are provided by the
// 'ball::LoggerManagerConfiguration' class to simply the definition of some of
// the attributes:
//..
// TBD: fix documentation xxxList
//  TYPE                                         'typedef' alias
//  ------------------------------------------   ------------------------------
//  bdlf::Function<void (*)(bdlmxxx::List *, bdlmxxx::Schema)>
//                                               UserPopulatorCallback
//
//  bdlf::Function<void (*)(bsl::string *, const char *)>
//                                               CategoryNameFilterCallback
//
//  bdlf::Function<void (*)(int *, int *, int *, int *, const char *)>
//                                               DefaultThresholdLevelsCallback
//..
// The attributes contained by a 'ball::LoggerManagerConfiguration' object
// follow:
//..
//  TYPE                                         NAME
//  ------------------------------------------   ------------------------------
//  ball::LoggerManagerDefaults                   defaults
//
//  bdlmxxx::Schema                              userSchema
//
//  UserPopulatorCallback                        userPopulatorCallback
//
//  CategoryNameFilterCallback                   categoryNameFilterCallback
//
//  DefaultThresholdLevelsCallback               defaultThresholdLevelsCallback
//
//  LogOrder                                     logOrder
//
//  TriggerMarkers                               triggerMarkers
//
//  NAME                            DESCRIPTION
//  -------------------             -------------------------------------------
//  defaults                        constrained defaults for buffer size and
//                                  thresholds
//
//  userSchema                      defines optional user-defined fields in a
//                                  log record
//
//  userPopulatorCallback           populates user-defined fields in a log
//                                  record
//
//  categoryNameFilterCallback      invoked on category names, e.g., to re-map
//                                  characters
//
//  defaultThresholdLevelsCallback  sets category severity threshold levels (by
//                                  default)
//
//  logOrder                        defines the order in which log messages are
//                                  published for Trigger and Trigger-All
//                                  events; default is LIFO (last-in first-out)
//
//  triggerMarkers                  defines whether text will be written to the
//                                  log to indicate whether a series of log
//                                  records were logged due to either a Trigger
//                                  or Trigger-All event; if this attribute is
//                                  'BAEL_BEGIN_END_MARKERS', then
//                                  "BEGIN RECORD DUMP" and "END RECORD DUMP"
//                                  will be written before and after each
//                                  sequence of records logged due to a Trigger
//                                  or Trigger-All event; default is
//                                  'BAEL_BEGIN_END_MARKERS'.
//..
// The constraints are as follows:
//..
//    NAME                             CONSTRAINT
//  +--------------------------------+--------------------------------+
//  | defaults                       | (a constrained-attribute type) |
//  +--------------------------------+--------------------------------+
//  | userSchema                     | (single attribute)             |
//  | userPopulatorCallback          |                                |
//  +--------------------------------+--------------------------------+
//  | categoryNameFilterCallback     | (none)                         |
//  +--------------------------------+--------------------------------+
//  | defaultThresholdLevelsCallback | (none)                         |
//  +--------------------------------+--------------------------------+
//  | logOrder                       | (none)                         |
//  +--------------------------------+--------------------------------+
//  | triggerMarkers                 | (none)                         |
//  +--------------------------------+--------------------------------+
//..
// For convenience, the 'ball::LoggerManagerConfiguration' interface contains
// manipulators and accessors to configure and inspect the value of its
// contained 'ball::LoggerManagerDefaults' object; these methods are identical
// to those of 'ball::LoggerManagerDefaults'.  See the
// 'ball_loggermanagerdefaults' component for details on the defaults and
// their constraints.
//
///Thread-Safety
///-------------
// This constrained-attribute component is *thread-safe* but not
// *thread-enabled*, and requires explicit synchronization in the user space.
// Note that any of the contained user-defined callbacks may be invoked from
// *any* thread, and the user must account for that.
//
///Usage
///-----
// The following snippets of code illustrate how to use a
// 'ball::LoggerManagerConfiguration' object.  First, we create a configuration
// object named 'config'.  Note that it is necessarily initialized to valid but
// unpublished defaults:
//..
//  ball::LoggerManagerConfiguration config;
//
//  assert(    0 == config.setDefaultRecordBufferSizeIfValid(32768));
//  assert(    0 == config.setDefaultLoggerBufferSizeIfValid(1024));
//  assert(    0 == config.setDefaultThresholdLevelsIfValid(192, 64, 48, 32));
//
//  assert(32768 == config.defaultRecordBufferSize());
//  assert( 1024 == config.defaultLoggerBufferSize());
//  assert(  192 == config.defaultRecordLevel());
//  assert(   64 == config.defaultPassLevel());
//  assert(   48 == config.defaultTriggerLevel());
//  assert(   32 == config.defaultTriggerAllLevel());
//
//  assert(ball::LoggerManagerConfiguration::BAEL_LIFO == config.logOrder());
//  assert(ball::LoggerManagerConfiguration::BAEL_BEGIN_END_MARKERS
//                                                 == config.triggerMarkers());
//..
// Next, set each attribute.  Note that the user schema and the corresponding
// user populator functor must be set atomically (i.e., with a single
// two-argument "set" method).  The user is responsible for the logical
// sensibility of the functor attributes, and especially the coherence of the
// schema and its populator.
//
// We will need to define a few objects before we can call the "set" methods.
// Also, to illustrate a non-null functor, we will create a trivial function
// payload 'pop' for the user populator functor attribute:
//..
//  void pop(bdlmxxx::List *list, bdlmxxx::Schema schema)
//  {
//  }
//..
// Now we can proceed to define the contents of our 'config' object.  We will
// need a schema and three functors, two of which will be default-constructed
// and not populated, resulting in "null" functors:
//..
//  bdlmxxx::Schema schema;
//  schema.createRecord("A");
//
//  UserPopulatorCallback          populator;
//  CategoryNameFilterCallback     nameFilter;
//  DefaultThresholdLevelsCallback defaultThresholds;
//
//  populator = &pop;
//..
// Note that the 'bdlf::Function' can be created with an optional allocator.
// The same allocator that is used to initialize the logger manager singleton
// (which is the global allocator if one is not explicitly supplied) should
// also be passed to 'bdlf::Function'.  In this simple example, we allow the
// default allocator to be used.
//
// We are now ready to populate our 'config' object, which is our goal.  Note
// that the "set" methods called in this example cannot fail, so they return
// 'void':
//..
//  config.setUserFieldDescriptors(schema, populator);
//  config.setCategoryNameFilterCallback(nameFilter);
//  config.setDefaultThresholdLevelsCallback(defaultThresholds);
//  config.setLogOrder(ball::LoggerManagerConfiguration::BAEL_FIFO);
//  config.setTriggerMarkers(ball::LoggerManagerConfiguration::BAEL_NO_MARKERS);
//
//  assert(           schema == config.userSchema());
//  // assert(        populator == config.userPopulatorCallback());
//  // assert(       nameFilter == config.categoryNameFilterCallback());
//  // assert(defaultThresholds == config.defaultThresholdLevelsCallback());
//  assert(ball::LoggerManagerConfiguration::BAEL_FIFO == config.logOrder());
//  assert(ball::LoggerManagerConfiguration::BAEL_NO_MARKERS
//                                                 == config.triggerMarkers());
//..
// The configuration object is now validly configured with our choice of
// parameters.  We can now print the configuration value to 'stdout':
//..
//  bsl::cout << config << bsl::endl;
//..
// This produces the following (multi-line) output:
//..
//  [
//      Defaults:
//      [
//          recordBufferSize : 32768
//          loggerBufferSize : 1024
//          recordLevel      : 192
//          passLevel        : 64
//          triggerLevel     : 48
//          triggerAllLevel  : 32
//      ]
//      User Fields Schema:
//      {
//          SEQUENCE RECORD "A" {
//          }
//      }
//      User Fields Populator functor is not null
//      Category Name Filter functor is null
//      Default Threshold Callback functor is null
//      Logging order is FIFO
//  ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_LOGGERMANAGERDEFAULTS
#include <ball_loggermanagerdefaults.h>
#endif

#ifndef INCLUDED_BALL_USERFIELDDESCRIPTORS
#include <ball_userfielddescriptors.h>
#endif

#ifndef INCLUDED_BDLF_FUNCTION
#include <bdlf_function.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace ball {     

class UserFieldValues;

                    // ================================
                    // class LoggerManagerConfiguration
                    // ================================

class LoggerManagerConfiguration {
    // This class provides constrained configuration parameters for a logger
    // manager.  The constraints are maintained as class invariants; it is not
    // possible to obtain an invalid object through this interface.
    //
    // This class support *in-core* *value-semantic* operations, including
    // copy construction, assignment, and equality comparison; 'ostream'
    // printing is supported, but in a modified form to handle functors, and
    // 'bdex' serialization cannot be supported at all.  (A precise operational
    // definition of when two instances have the same (in-core) value can be
    // found in the description of 'operator==' for the class.)  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

  public:
    // PUBLIC TYPES
    typedef bdlf::Function<void (*)(ball::UserFieldValues *, 
                                    const ball::UserFieldDescriptors& )>
                                                   UserFieldsPopulatorCallback;
        // 'UserFieldsPopulatorCallback' is the type of a user-supplied
        // callback functor used to populate the user-defined fields in each
        // log record.  Note that the user-defined fields of each record must
        // be type-consistent with the 'UserFieldDescriptors' of the user
        // populator callback.


    typedef bdlf::Function<void (*)(bsl::string *, const char *)>
                                                    CategoryNameFilterCallback;
        // 'CategoryNameFilterCallback' is the type of the user-supplied
        // functor that translates external category names to internal names.

    typedef bdlf::Function<void (*)(int *, int *, int *, int *, const char *)>
                                                DefaultThresholdLevelsCallback;
        // 'DefaultThresholdLevelsCallback' is the type of the functor that
        // determines default threshold levels for categories added to the
        // registry by the 'setCategory(const char *)' method.

    enum LogOrder {
        // The 'LogOrder' enumeration defines the order in which messages
        // will be published to the underlying observer in the case of
        // Trigger and Trigger-All events.

//ARB:ENUM 316
        BAEL_FIFO,  // oldest logged messages are published first
//ARB:ENUM 317
        BAEL_LIFO   // newest logged messages are published first

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , FIFO = BAEL_FIFO
      , LIFO = BAEL_LIFO
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum TriggerMarkers {
        // The 'TriggerMarkers' enumeration defines whether text will be
        // written to the log to indicate whether a series of log records were
        // logged due to either a Trigger or Trigger-All event.  If this
        // attribute is 'BAEL_BEGIN_END_MARKERS', then "BEGIN RECORD DUMP" and
        // "END RECORD DUMP" will be written before and after each sequence of
        // records logged due to a Trigger or Trigger-All event.  The default
        // value of this attribute is 'BAEL_BEGIN_END_MARKERS'.

//ARB:ENUM 334
        BAEL_NO_MARKERS,        // don't print any markers

//ARB:ENUM 336
        BAEL_BEGIN_END_MARKERS  // print "BEGIN RECORD DUMP" and
                                // "END RECORD DUMP" markers (default)

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , NO_MARKERS        = BAEL_NO_MARKERS
      , BEGIN_END_MARKERS = BAEL_BEGIN_END_MARKERS
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    // DATA
    LoggerManagerDefaults
                          d_defaults;             // default buffer size for
                                                  // logger (for macros) and
                                                  // set of constrained default
                                                  // severity threshold levels
                                                  // for logger manager

    ball::UserFieldDescriptors  
                          d_userFieldDescriptors; // describes the fields
                                                  // returned by 
                                                  // 'd_userPopulatorCallback'

    UserFieldsPopulatorCallback 
                          d_userPopulator;        // user callback to add
                                                  // optional user-defined
                                                  // fields to a log record

    CategoryNameFilterCallback
                          d_categoryNameFilter;   // callback to modify input
                                                  // category names for
                                                  // internal storage

    DefaultThresholdLevelsCallback
                          d_defaultThresholdsCb;  // callback to supply default
                                                  // severity thresholds for
                                                  // categories

    LogOrder              d_logOrder;             // logging order

    TriggerMarkers        d_triggerMarkers;       // trigger marker

    bslma::Allocator     *d_allocator_p;          // memory allocator (held,
                                                  // not owned)

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&,
                                    const LoggerManagerConfiguration&);

    friend bool operator==(const LoggerManagerConfiguration&,
                           const LoggerManagerConfiguration&);

  public:
    // CLASS METHODS
    static bool isValidDefaultRecordBufferSize(int numBytes);
        // Return 'true' if the specified 'numBytes' is a valid default-logger
        // record-buffer size value, and 'false' otherwise.  'numBytes' is
        // valid if '0 < numBytes'.

    static bool isValidDefaultLoggerBufferSize(int numBytes);
        // Return 'true' if the specified 'numBytes' is a valid default
        // logger-message-buffer size value, and 'false' otherwise.  'numBytes'
        // is valid if '0 < numBytes'.

    static bool areValidDefaultThresholdLevels(int recordLevel,
                                               int passLevel,
                                               int triggerLevel,
                                               int triggerAllLevel);
        // Return 'true' if each of the specified 'recordLevel', 'passLevel',
        // 'triggerLevel', and 'triggerAllLevel' values is a valid default
        // severity threshold level, and 'false' otherwise.  Valid severity
        // threshold levels are in the range '[0 .. 255]'.

    // CREATORS
    explicit LoggerManagerConfiguration(
                                         bslma::Allocator *basicAllocator = 0);
        // Create a logger manager configuration constrained-attribute object
        // having valid default values for all attributes.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    LoggerManagerConfiguration(
                   const LoggerManagerConfiguration&  original,
                   bslma::Allocator                       *basicAllocator = 0);
        // Create a logger manager configuration constrained-attribute object
        // having the in-core value of the specified 'original' object.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~LoggerManagerConfiguration();
        // Destroy this logger manager configuration constrained-attribute
        // object.

    // MANIPULATORS
    LoggerManagerConfiguration&
                         operator=(const LoggerManagerConfiguration& rhs);
        // Assign to this logger manager configuration constrained-attribute
        // object the in-core value of the specified 'rhs' object.

    void setDefaultValues(const LoggerManagerDefaults& defaults);
        // Set the default values attribute of this object to the specified
        // 'defaults'.  Note that, since 'defaults' is itself a
        // constrained-attribute object, this method cannot fail.

    int setDefaultRecordBufferSizeIfValid(int numBytes);
        // Set the default-logger record-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object to the
        // specified 'numBytes' if '0 < numBytes'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on this object.

    int setDefaultLoggerBufferSizeIfValid(int numBytes);
        // Set the default logger-message-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object to the
        // specified 'numBytes' if '0 < numBytes'.  Return 0 on success, and a
        // non-zero value otherwise with no effect on this object.

    int setDefaultThresholdLevelsIfValid(int passLevel);
        // Set the passthrough severity threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object to the 
        // specified 'passLevel', if it is in the range '[0 .. 255]', and set
        // all the other threshold levels (recordLevel, triggerLevel,
        // triggerAllLevel) to 0.  Return 0 on success, and a non-zero value
        // otherwise with no effect on this object.

    int setDefaultThresholdLevelsIfValid(int recordLevel,
                                         int passLevel,
                                         int triggerLevel,
                                         int triggerAllLevel);
        // Set the quadruplet of default severity threshold level attributes of
        // the 'LoggerManagerDefaults' attribute of this object to the
        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' values if each level is in the range '[0 .. 255]'.
        // Return 0 on success, and a non-zero value otherwise with no effect
        // on this object.
    
    void setUserFieldDescriptors(
                       const ball::UserFieldDescriptors   fieldDescriptions,
                       const UserFieldsPopulatorCallback& populatorCallback);
        // Set the user-defined-fields attributes of this object such that the
        // specified 'populatorCallback' will be invoked and supplied the
        // specified 'fieldDescriptions'.  Note that this method cannot
        // fail per se, but it is the user's responsibility to make sure that
        // 'populatorCallback' can populate a 'ball::UserFieldValues' object
        // in a way consistent with the 'fieldDescriptions'.

    void setCategoryNameFilterCallback(
                                 const CategoryNameFilterCallback& nameFilter);
        // Set the category name-filter callback functor attribute of this
        // object to the specified 'nameFilter'.  Note that this method cannot
        // fail, and that 'nameFilter' can be a "null" (i.e., unpopulated)
        // functor object.

    void setDefaultThresholdLevelsCallback(
                           const DefaultThresholdLevelsCallback& thresholdsCb);
        // Set the default thresholds callback functor attribute of this object
        // to the specified 'thresholdsCb'.  Note that this method cannot fail,
        // and that 'thresholdsCb' can be a "null" (i.e., unpopulated) functor
        // object.

    void setLogOrder(LogOrder value);
        // Set the log order attribute of this object to the specified 'value'.

    void setTriggerMarkers(TriggerMarkers value);
        // Set the trigger marker attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    const LoggerManagerDefaults& defaults() const;
        // Return a reference to the non-modifiable defaults object attribute
        // of this object.

    int defaultRecordBufferSize() const;
        // Return the default-logger record-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultLoggerBufferSize() const;
        // Return the default logger-message-buffer size attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultRecordLevel() const;
        // Return the default Record threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultPassLevel() const;
        // Return the default Pass threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultTriggerLevel() const;
        // Return the default Trigger threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.

    int defaultTriggerAllLevel() const;
        // Return the default Trigger-All threshold level attribute of the
        // 'LoggerManagerDefaults' attribute of this object.


    const ball::UserFieldDescriptors& userFieldDescriptors() const;
        // Return a reference to the non-modifiable descriptors for user
        // fields.

    const UserFieldsPopulatorCallback& userFieldsPopulatorCallback() const;
        // Return a reference to the non-modifiable user populator functor
        // attribute of this object.

    const CategoryNameFilterCallback& categoryNameFilterCallback() const;
        // Return a reference to the non-modifiable category name filter
        // functor attribute of this object.

    const DefaultThresholdLevelsCallback&
                                        defaultThresholdLevelsCallback() const;
        // Return a reference to the non-modifiable default severity-level
        // thresholds functor attribute of this object.

    LogOrder logOrder() const;
        // Return the log order attribute of this object.  See attributes
        // description for effects of the log order.

    TriggerMarkers triggerMarkers() const;
        // Return the trigger marker attribute of this object.  See attributes
        // description for effects of the trigger markers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format a reasonable representation of this object to the specified
        // output 'stream' at the (absolute value of) the optionally specified
        // indentation 'level' and return a reference to 'stream'.  If 'level'
        // is specified, optionally specify 'spacesPerLevel', the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress all indentation AND format
        // the entire output on one line.  If 'stream' is not valid on entry,
        // this operation has no effect.  The format will indicate whether the
        // contained functors are or are not "null", but will not otherwise
        // attempt to represent functor "values".
};

// FREE OPERATORS
bool operator==(const LoggerManagerConfiguration& lhs,
                const LoggerManagerConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same (in-core) value, and 'false' otherwise.  Two attribute objects
    // objects have the same in-core value if each respective attribute has the
    // same in-core value.  Note that functor attributes must have identically
    // the same representation to have the same in-core value.

bool operator!=(const LoggerManagerConfiguration& lhs,
                const LoggerManagerConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same (in-core) value, and 'false' otherwise.  Two attribute
    // objects do not have the same in-core value if one or more respective
    // attributes differ in in-core values.  Note that functor attributes will
    // have different in-core values if their representations are not
    // identical.

bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const LoggerManagerConfiguration& configuration);
}  // close package namespace
    // Write a reasonable representation of the specified 'configuration'
    // object to the specified output 'stream', indicating whether the
    // contained functors are or are not "null".

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
