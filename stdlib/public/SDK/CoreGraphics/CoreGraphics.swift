//===----------------------------------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

@_exported import CoreGraphics
import Darwin

//===----------------------------------------------------------------------===//
// CGGeometry
//===----------------------------------------------------------------------===//

public extension CGPoint {
  static var zero: CGPoint {
    @_transparent // @fragile
    get { return CGPoint(x: 0, y: 0) }
  }

  @_transparent // @fragile
  init(x: Int, y: Int) {
    self.init(x: CGFloat(x), y: CGFloat(y))
  }

  @_transparent // @fragile
  init(x: Double, y: Double) {
    self.init(x: CGFloat(x), y: CGFloat(y))
  }
}

extension CGPoint : CustomReflectable, CustomPlaygroundQuickLookable {
  public var customMirror: Mirror {
    return Mirror(self, children: ["x": x, "y": y], displayStyle: .`struct`)
  }

  public var customPlaygroundQuickLook: PlaygroundQuickLook {
    return .point(Double(x), Double(y))
  }
}

extension CGPoint : CustomDebugStringConvertible {
  public var debugDescription: String {
    return "(\(x), \(y))"
  }
}

extension CGPoint : Equatable {}
@_transparent // @fragile
@warn_unused_result
public func == (lhs: CGPoint, rhs: CGPoint) -> Bool {
  return lhs.x == rhs.x  &&  lhs.y == rhs.y
}

public extension CGSize {
  static var zero: CGSize {
    @_transparent // @fragile
    get { return CGSize(width: 0, height: 0) }
  }

  @_transparent // @fragile
  init(width: Int, height: Int) {
    self.init(width: CGFloat(width), height: CGFloat(height))
  }

  @_transparent // @fragile
  init(width: Double, height: Double) {
    self.init(width: CGFloat(width), height: CGFloat(height))
  }
}

extension CGSize : CustomReflectable, CustomPlaygroundQuickLookable {
  public var customMirror: Mirror {
    return Mirror(
      self,
      children: ["width": width, "height": height],
      displayStyle: .`struct`)
  }

  public var customPlaygroundQuickLook: PlaygroundQuickLook {
    return .size(Double(width), Double(height))
  }
}

extension CGSize : CustomDebugStringConvertible {
  public var debugDescription : String {
    return "(\(width), \(height))"
  }
}

extension CGSize : Equatable {}
@_transparent // @fragile
@warn_unused_result
public func == (lhs: CGSize, rhs: CGSize) -> Bool {
  return lhs.width == rhs.width  &&  lhs.height == rhs.height
}

public extension CGVector {
  static var zero: CGVector {
    @_transparent // @fragile
    get { return CGVector(dx: 0, dy: 0) }
  }

  @_transparent // @fragile
  init(dx: Int, dy: Int) {
    self.init(dx: CGFloat(dx), dy: CGFloat(dy))
  }

  @_transparent // @fragile
  init(dx: Double, dy: Double) {
    self.init(dx: CGFloat(dx), dy: CGFloat(dy))
  }
}

extension CGVector : Equatable {}
@_transparent // @fragile
@warn_unused_result
public func == (lhs: CGVector, rhs: CGVector) -> Bool {
  return lhs.dx == rhs.dx  &&  lhs.dy == rhs.dy
}


public extension CGRect {
  static var zero: CGRect {
    @_transparent // @fragile
    get { return CGRect(x: 0, y: 0, width: 0, height: 0) }
  }
  static var null: CGRect {
    @_transparent // @fragile
    get { return CGRectNull }
  }
  static var infinite: CGRect {
    @_transparent // @fragile
    get { return CGRectInfinite }
  }

  @_transparent // @fragile
  init(x: CGFloat, y: CGFloat, width: CGFloat, height: CGFloat) {
    self.init(origin: CGPoint(x: x, y: y),
              size: CGSize(width: width, height: height))
  }

  @_transparent // @fragile
  init(x: Double, y: Double, width: Double, height: Double) {
    self.init(origin: CGPoint(x: x, y: y),
              size: CGSize(width: width, height: height))
  }

  @_transparent // @fragile
  init(x: Int, y: Int, width: Int, height: Int) {
    self.init(origin: CGPoint(x: x, y: y),
              size: CGSize(width: width, height: height))
  }

  var width:  CGFloat {
    @_transparent // @fragile
    get { return CGRectGetWidth(self) }
  }
  var height: CGFloat {
    @_transparent // @fragile
    get { return CGRectGetHeight(self) }
  }

  var minX:   CGFloat {
    @_transparent // @fragile
    get { return CGRectGetMinX(self) }
  }
  var midX:   CGFloat {
    @_transparent // @fragile
    get { return CGRectGetMidX(self) }
  }
  var maxX:   CGFloat {
    @_transparent // @fragile
    get { return CGRectGetMaxX(self) }
  }
  var minY:   CGFloat {
    @_transparent // @fragile
    get { return CGRectGetMinY(self) }
  }
  var midY:   CGFloat {
    @_transparent // @fragile
    get { return CGRectGetMidY(self) }
  }
  var maxY:   CGFloat {
    @_transparent // @fragile
    get { return CGRectGetMaxY(self) }
  }
  var isNull:     Bool {
    @_transparent // @fragile
    get { return CGRectIsNull(self) }
  }
  var isEmpty:    Bool {
    @_transparent // @fragile
    get { return CGRectIsEmpty(self) }
  }
  var isInfinite: Bool {
    @_transparent // @fragile
    get { return CGRectIsInfinite(self) }
  }
  var standardized: CGRect {
    @_transparent // @fragile
    get { return CGRectStandardize(self) }
  }

  var integral: CGRect {
    @_transparent // @fragile
    get { return CGRectIntegral(self) }
  }

  @_transparent // @fragile
  mutating func standardizeInPlace() {
    self = standardized
  }

  @_transparent // @fragile
  mutating func makeIntegralInPlace() {
    self = integral
  }

  @_transparent // @fragile
  @warn_unused_result(mutable_variant: "insetInPlace")
  func insetBy(dx dx: CGFloat, dy: CGFloat) -> CGRect {
    return CGRectInset(self, dx, dy)
  }

  @_transparent // @fragile
  mutating func insetInPlace(dx dx: CGFloat, dy: CGFloat) {
    self = insetBy(dx: dx, dy: dy)
  }

  @_transparent // @fragile
  @warn_unused_result(mutable_variant: "offsetInPlace")
  func offsetBy(dx dx: CGFloat, dy: CGFloat) -> CGRect {
    return CGRectOffset(self, dx, dy)
  }

  @_transparent // @fragile
  mutating func offsetInPlace(dx dx: CGFloat, dy: CGFloat) {
    self = offsetBy(dx: dx, dy: dy)
  }

  @_transparent // @fragile
  @warn_unused_result(mutable_variant: "formUnion")
  func union(rect: CGRect) -> CGRect {
    return CGRectUnion(self, rect)
  }

  @_transparent // @fragile
  mutating func formUnion(rect: CGRect) {
    self = union(rect)
  }

  @_transparent // @fragile
  @warn_unused_result(mutable_variant: "formIntersection")
  func intersection(rect: CGRect) -> CGRect {
    return CGRectIntersection(self, rect)
  }

  @_transparent // @fragile
  mutating func formIntersection(rect: CGRect) {
    self = intersection(rect)
  }

  @_transparent // @fragile
  @warn_unused_result
  func divide(atDistance: CGFloat, fromEdge: CGRectEdge)
    -> (slice: CGRect, remainder: CGRect)
  {
    var slice = CGRect.zero
    var remainder = CGRect.zero
    CGRectDivide(self, &slice, &remainder, atDistance, fromEdge)
    return (slice, remainder)
  }

  @_transparent // @fragile
  @warn_unused_result
  func contains(rect: CGRect) -> Bool {
    return CGRectContainsRect(self, rect)
  }

  @_transparent // @fragile
  @warn_unused_result
  func contains(point: CGPoint) -> Bool {
    return CGRectContainsPoint(self, point)
  }

  @_transparent // @fragile
  @warn_unused_result
  func intersects(rect: CGRect) -> Bool {
    return CGRectIntersectsRect(self, rect)
  }
}

extension CGRect : CustomReflectable, CustomPlaygroundQuickLookable {
  public var customMirror: Mirror {
    return Mirror(
      self,
      children: ["origin": origin, "size": size],
      displayStyle: .`struct`)
  }

  public var customPlaygroundQuickLook: PlaygroundQuickLook {
    return .rectangle(
      Double(origin.x), Double(origin.y),
      Double(size.width), Double(size.height))
  }
}

extension CGRect : CustomDebugStringConvertible {
  public var debugDescription : String {
    return "(\(origin.x), \(origin.y), \(size.width), \(size.height))"
  }
}

extension CGRect : Equatable {}
@_transparent // @fragile
@warn_unused_result
public func == (lhs: CGRect, rhs: CGRect) -> Bool {
  return CGRectEqualToRect(lhs, rhs)
}

// Overlay the C names of these constants with transparent definitions. The
// C constants are opaque extern globals for no good reason.

public var CGPointZero: CGPoint {
  @_transparent // @fragile
  get { return CGPoint.zero }
}

public var CGRectZero: CGRect {
  @_transparent // @fragile
  get { return CGRect.zero }
}

public var CGSizeZero: CGSize {
  @_transparent // @fragile
  get { return CGSize.zero }
}

public var CGAffineTransformIdentity: CGAffineTransform {
  @_transparent // @fragile
  get { return CGAffineTransform(a: 1, b: 0, c: 0, d: 1, tx: 0, ty: 0) }
}
