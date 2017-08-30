#include "RunAsAdmin.hpp"
#include "ResourcePath.hpp"
#import <Foundation/Foundation.h>

////////////////////////////////////////////////////////////
bool runAsAdmin(std::string cmd)
{
    cmd = "do shell script \"" + cmd + "\" with administrator privileges";
    NSString *script = [NSString stringWithCString:cmd.c_str() encoding:[NSString defaultCStringEncoding]];
    NSDictionary *error = [NSDictionary new];
    NSAppleScript *appleScript = [[NSAppleScript alloc] initWithSource:script];
    if ([appleScript executeAndReturnError:&error]) {
        return true;
    } else {
        return false;
    }
}
