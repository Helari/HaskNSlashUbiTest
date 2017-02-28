// Fill out your copyright notice in the Description page of Project Settings.

#include "HackNSlash.h"
#include "HUDBlueprintLibrary.h"

void UHUDBlueprintLibrary::FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const float EdgePercent, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool &bIsOnScreen, bool &bIsBehind, FString &debugOutpus)
{
	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	debugOutpus = "";
	if (!GEngine) return;

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);

	if (!World) return;

	APlayerController* PlayerController = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);

	if (!PlayerController) return;

	ACharacter* PlayerCharacter = Cast<ACharacter>(PlayerController->GetPawn());

	if (!PlayerCharacter) return;

	APlayerCameraManager* PlayerCamera = (WorldContextObject ? UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0) : NULL);
	
	FVector ScreenPos;
	
	ProjectWorldToScreen(PlayerCharacter->GetNetOwningPlayer(), InLocation, ScreenPos);

	bIsBehind = ScreenPos.Z < 0.0f;
	
	// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.	
	if (ScreenPos.X >= (0.f + ViewportCenter.X*EdgePercent) && ScreenPos.X <= (ViewportSize.X - ViewportCenter.X*EdgePercent)
		&& ScreenPos.Y >= (0.f + ViewportCenter.Y*EdgePercent) && ScreenPos.Y <= (ViewportSize.Y - ViewportCenter.Y*EdgePercent) && !bIsBehind)
	{

		bIsOnScreen = true;
		OutScreenPosition.X = ScreenPos.X;
		OutScreenPosition.Y = ScreenPos.Y;
		return;
	}

	ScreenPos -= FVector(ViewportCenter.X, ViewportCenter.Y, 0.0f);
	float AngleRadians = FMath::Atan2(ScreenPos.Y, ScreenPos.X);
	AngleRadians -= FMath::DegreesToRadians(90.f);
	OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;
	ScreenPos += FVector(ViewportCenter.X, ViewportCenter.Y, 0.0f);
			
	if (bIsBehind)
	{
		OutRotationAngleDegrees -= 180.f;
		if (OutRotationAngleDegrees > -90.0f)
		{
			OutRotationAngleDegrees = (OutRotationAngleDegrees + 180.0f) * -1.0f;
			debugOutpus = "lol";
		}
		ScreenPos.X = ViewportSize.X - ScreenPos.X;
		ScreenPos.Y = ViewportSize.Y;
	}

	if (ScreenPos.X < 0 + ViewportCenter.X*EdgePercent)
	{
		ScreenPos.X = 0 + ViewportCenter.X*EdgePercent;
	}
	if (ScreenPos.X > ViewportSize.X - ViewportCenter.X*EdgePercent)
	{
		ScreenPos.X = ViewportSize.X - ViewportCenter.X*EdgePercent;
	}
	if (ScreenPos.Y < 0 + ViewportCenter.Y*EdgePercent)
	{
		ScreenPos.Y = 0 + ViewportCenter.Y*EdgePercent;
	}
	if (ScreenPos.Y > ViewportSize.Y - ViewportCenter.Y*EdgePercent)
	{
		ScreenPos.Y = ViewportSize.Y - ViewportCenter.Y*EdgePercent;
	}
	OutScreenPosition.X = ScreenPos.X;
	OutScreenPosition.Y = ScreenPos.Y;
}

//void AShooterHUD::DrawObjective()
//{
//	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetOwningPawn());
//	AShooterGameState* GameState = Cast<AShooterGameState>(GetWorld()->GetGameState());
//	AShooterPlayerState* MyPlayerState = nullptr;
//	if (MyPawn)
//	{
//		MyPlayerState = Cast<AShooterPlayerState>(GetOwningPawn()->PlayerState);
//	}
//	if (!MyPlayerState)
//	{
//		AShooterSpectatorPawn* SpecPawn = Cast<AShooterSpectatorPawn>(PlayerOwner->GetSpectatorPawn());
//		if (SpecPawn && SpecPawn->GetFollowedCharacter())
//		{
//			MyPawn = Cast<AShooterCharacter>(SpecPawn->GetFollowedCharacter());
//			MyPlayerState = Cast<AShooterPlayerState>(SpecPawn->GetFollowedCharacter()->PlayerState);
//		}
//	}
//
//	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
//
//		FVector WorldPos;
//		FVector ScreenPos;
//					const float StartScaleDist = 2000.f;
//					const float BallIconMinScale = 0.8f;
//					const float BallIconMaxScale = 1.0f;
//					const float OffsetZ = 5.f;
//
//					WorldPos = InLocation;
//					WorldPos.Z += OffsetZ;
//
//					float dist = FVector::Dist(WorldPos, PlayerCharacter->GetActorLocation());
//					float distScale = FMath::Min(dist / StartScaleDist, BallIconMaxScale);
//					if (distScale < BallIconMinScale) distScale = BallIconMinScale;
//
//					
//					bool projected = PlayerController->ProjectWorldLocationToScreen(WorldPos, ScreenPos);//*ScreenPosition);//ProjectWorldToScreen(PlayerOwner->GetNetOwningPlayer(), WorldPos, ScreenPos);
//					if (projected && ScreenPos.Z > 0.0f)
//					{
//						ScreenPos.X -= Icon.UL / 2.f * distScale * ScaleUI;
//						ScreenPos.Y -= Icon.VL * distScale * ScaleUI;
//					}
//
//					if (ScreenPos.Z < 0.0f)
//					{
//						ScreenPos.X = ViewportSize.X - ScreenPos.X;
//						ScreenPos.Y = ViewportSize.Y - Icon.VL * distScale * ScaleUI;
//					}
//					if (ScreenPos.X < 0)
//					{
//						ScreenPos.X = 0;
//					}
//					if (ScreenPos.X > ViewportSize.X - Icon.UL * distScale * ScaleUI)
//					{
//						ScreenPos.X = ViewportSize.X - Icon.UL * distScale * ScaleUI;
//					}
//					if (ScreenPos.Y < 0)
//					{
//						ScreenPos.Y = 0;
//					}
//					if (ScreenPos.Y > ViewportSize.Y - Icon.UL * distScale * ScaleUI)
//					{
//						ScreenPos.Y = ViewportSize.Y - Icon.VL * distScale * ScaleUI;
//					}
//
//					//Canvas->SetDrawColor(BallIndicatorHUDColor);
//					//Canvas->DrawIcon(ObjectiveBallIcon, ScreenPos.X, ScreenPos.Y, ScaleUI * distScale);
//					OutScreenPosition = ScreenPos;
//				}
//			}
//		}
//
//
//		AShooterGame_Goal* Goal = GameState->GetGoal(MyPlayerState->GetTeamNum());
//		if (Goal)
//		{
//			const float StartScaleDist = 5000.f;
//			const float GoalIconMinScale = 0.7f;
//			const float GoalIconMaxScale = 0.8f;
//
//			WorldPos = Goal->GetActorLocation();
//			WorldPos.Z += Goal->GetSimpleCollisionHalfHeight() * 1.25f;
//
//
//			float dist = FVector::Dist(WorldPos, MyPawn->GetActorLocation());
//			float distScale = FMath::Min(dist / StartScaleDist, GoalIconMaxScale);
//			if (distScale < GoalIconMinScale) distScale = GoalIconMinScale;
//
//			bool projected = ProjectWorldToScreen(PlayerOwner->GetNetOwningPlayer(), WorldPos, ScreenPos);
//			if (projected && ScreenPos.Z > 0.0f)
//			{
//				ScreenPos.X -= ObjectiveBallIcon.UL / 2.f * distScale * ScaleUI;
//				ScreenPos.Y -= ObjectiveBallIcon.VL * distScale * ScaleUI;
//			}
//
//			if (ScreenPos.Z < 0.0f) {
//				ScreenPos.X = ViewportSize.X - ScreenPos.X;
//				ScreenPos.Y = ViewportSize.Y - ObjectiveGoalIcon.VL * distScale;
//			}
//			if (ScreenPos.Z < 0.0f)
//			{
//				ScreenPos.X = ViewportSize.X - ScreenPos.X;
//				ScreenPos.Y = ViewportSize.Y - ObjectiveGoalIcon.VL * distScale * ScaleUI;
//			}
//			if (ScreenPos.X < 0)
//			{
//				ScreenPos.X = 0;
//			}
//			if (ScreenPos.X > ViewportSize.X - ObjectiveGoalIcon.UL * distScale * ScaleUI)
//			{
//				ScreenPos.X = ViewportSize.X - ObjectiveGoalIcon.UL * distScale * ScaleUI;
//			}
//			if (ScreenPos.Y < 0)
//			{
//				ScreenPos.Y = 0;
//			}
//			if (ScreenPos.Y > ViewportSize.Y - ObjectiveGoalIcon.UL * distScale * ScaleUI)
//			{
//				ScreenPos.Y = ViewportSize.Y - ObjectiveGoalIcon.VL * distScale * ScaleUI;
//			}
//
//			if (MyPlayerState)
//			{
//				if (MyPlayerState->GetTeamNum() == 0)
//				{
//					Canvas->SetDrawColor(OrangeTeamHUDColor);
//				}
//				else if (MyPlayerState->GetTeamNum() == 1)
//				{
//					Canvas->SetDrawColor(BlueTeamHUDColor);
//				}
//			}
//			Canvas->DrawIcon(ObjectiveGoalIcon, ScreenPos.X, ScreenPos.Y, ScaleUI*distScale);
//		}
//	}
//}

bool UHUDBlueprintLibrary::ProjectWorldToScreen(UPlayer* Player, FVector WorldLocation, FVector& ScreenLocation)
{
	// Code from: APlayerController::ProjectWorldLocationToScreen 
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer != NULL && LocalPlayer->ViewportClient != NULL && LocalPlayer->ViewportClient->Viewport != NULL)
	{
		// Create a view family for the game viewport
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			LocalPlayer->GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		// Calculate a view where the player is to update the streaming from the players start location
		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, /*out*/ ViewLocation, /*out*/ ViewRotation, LocalPlayer->ViewportClient->Viewport);

		if (SceneView)
		{
			// This does not return the desired result
			//return SceneView->WorldToPixel(WorldLocation, ScreenLocation);

			FIntPoint size = LocalPlayer->ViewportClient->Viewport->GetSizeXY();
			int ClipX = size.X;
			int ClipY = size.Y;

			// Code taken from UCanvas::Project
			FPlane V = SceneView->Project(WorldLocation);
			FVector resultVec(V);
			resultVec.X = (ClipX / 2.f) + (resultVec.X*(ClipX / 2.f));
			resultVec.Y *= -1.f * 1.0f;
			resultVec.Y = (ClipY / 2.f) + (resultVec.Y*(ClipY / 2.f));

			// if behind the screen, clamp depth to the screen
			/*if (V.W <= 0.0f)
			{
			resultVec.Z = 0.0f;
			}*/
			

			ScreenLocation.X = resultVec.X;
			ScreenLocation.Y = resultVec.Y;
			ScreenLocation.Z = resultVec.Z;
			return true;
		}
	}
	return false;
}



////static ConstructorHelpers::FObjectFinder<UTexture2D> ObjectiveBallOb(TEXT("/Game/UI/off_indicator"));
//UTexture2D* ObjectiveTexture = Icon;
//FCanvasIcon ObjectiveIcon = UCanvas::MakeIcon(ObjectiveTexture);
//
//bIsOnScreen = false;
//OutRotationAngleDegrees = 0.f;
//
//if (!GEngine) return;
//
//const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
//const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
//FVector CamForward = PlayerCharacter->GetActorLocation() - PlayerCamera->GetCameraLocation();
//FVector2D Forward = FVector2D(CamForward.X, CamForward.Y);
//FVector2D Offset = ((FVector2D)InLocation - (FVector2D)PlayerCamera->GetActorForwardVector());
//
//float DotProduct = FVector2D::DotProduct(Forward.GetSafeNormal(), Offset.GetSafeNormal());
//bool bLocationIsBehindCamera = (DotProduct <= 0.2f);
//Dot1 = DotProduct;
//bIsBehind = bLocationIsBehindCamera;
//FVector WorldPos;
//FVector ScreenPos;
//const float StartScaleDist = 1.f;
//const float BallIconMinScale = 1.f;
//const float BallIconMaxScale = 1.0f;
//const float OffsetZ = 0.f;
//const float ScaleUI = 1.f;
//
//WorldPos = InLocation;
//WorldPos.Z += OffsetZ;
//
//float dist = FVector::Dist(WorldPos, PlayerCharacter->GetActorLocation());
//float distScale = FMath::Min(dist / StartScaleDist, BallIconMaxScale);
//if (distScale < BallIconMinScale) distScale = BallIconMinScale;
//
//bool projected = ProjectWorldToScreen(PlayerCharacter->GetNetOwningPlayer(), InLocation, ScreenPos);//PlayerController->ProjectWorldLocationToScreen(WorldPos, ScreenPos);//*ScreenPosition);//
//PlayerController->ProjectWorldLocationToScreen(InLocation, OutScreenPosition);//*ScreenPosition);
//
//																			  // Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.	
//if (ScreenPos.X >= (0.f + ViewportCenter.X*EdgePercent) && ScreenPos.X <= (ViewportSize.X - ViewportCenter.X*EdgePercent)
//	&& ScreenPos.Y >= (0.f + ViewportCenter.Y*EdgePercent) && ScreenPos.Y <= (ViewportSize.Y - ViewportCenter.Y*EdgePercent) && !bLocationIsBehindCamera)
//{
//
//	bIsOnScreen = true;
//	return;
//}
//if (bLocationIsBehindCamera)
//{
//	// For behind the camera situation, we cheat a little to put the
//	// marker at the bottom of the screen so that it moves smoothly
//	// as you turn around. Could stand some refinement, but results
//	// are decent enough for most purposes.
//	FVector2D InLocation2D = (FVector2D)InLocation;
//	FVector2D PlayerLocation2D = (FVector2D)PlayerCharacter->GetActorLocation();
//	FVector DiffVector = FVector(InLocation2D.X, InLocation2D.Y, 0.0f) - FVector(PlayerLocation2D.X, PlayerLocation2D.Y, 0.0f);// InLocation - PlayerCharacter->GetActorLocation();
//	FVector Inverted = DiffVector * -1.f;
//	FVector NewInLocation = FVector(InLocation2D.X, InLocation2D.Y, 0.0f) + DiffVector;
//	NewInLocation.Z += PlayerCharacter->GetActorLocation().Z;
//	//NewInLocation.Z -= 5000;
//
//	//PlayerController->ProjectWorldLocationToScreen(NewInLocation, OutScreenPosition);
//	ProjectWorldToScreen(PlayerCharacter->GetNetOwningPlayer(), NewInLocation, ScreenPos);
//	//	ScreenPos.Y = ViewportSize.Y;
//	//	ScreenPos.X = ViewportSize.X - ScreenPos.X;
//	//dot = x1*x2 + y1*y2      # dot product
//	FVector2D x = Forward.GetSafeNormal();
//	FVector2D y = Offset.GetSafeNormal();
//	float dot = x.X*x.Y - y.X*y.Y;//x1*x2 + y1*y2      # dot product
//	Dot2 = dot;
//	float det = x.X*y.Y - y.X*x.Y;//x1*y2 - y1*x2      # determinant
//	float angle = atan2(det, dot);
//	float angleSign = 1.0f;
//	if (angle < 0.0f)
//	{
//		angleSign = -1.0f;
//	}
//	//ScreenPos.X = ViewportCenter.X - ((1 - dot)*(ViewportSize.X/2.f)) * angleSign;
//}

//OutScreenPosition -= ViewportCenter;
//float AngleRadians = FMath::Atan2(OutScreenPosition.Y, OutScreenPosition.X);
//AngleRadians -= FMath::DegreesToRadians(90.f);
//OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;
//distScale = 1.0f;
//if (projected && ScreenPos.Z > 0.0f)
//{
//	bIsBehind = false;
//	//ScreenPos.X -= ObjectiveIcon.UL * distScale * ScaleUI;
//	//ScreenPos.Y -= ObjectiveIcon.VL * distScale * ScaleUI;
//}
//
//if (ScreenPos.Z < 0.0f)
//{
//	bIsBehind = true;
//	ScreenPos.X = ViewportSize.X - ScreenPos.X;
//	ScreenPos.Y = ViewportSize.Y;// -ObjectiveIcon.VL * distScale * ScaleUI;
//}
//
//if (ScreenPos.X < 0 + ViewportCenter.X*EdgePercent)
//{
//	ScreenPos.X = 0 + ViewportCenter.X*EdgePercent;
//}
//if (ScreenPos.X > ViewportSize.X - ViewportCenter.X*EdgePercent)// - ObjectiveIcon.UL * distScale * ScaleUI)
//{
//	ScreenPos.X = ViewportSize.X - ViewportCenter.X*EdgePercent;// - ObjectiveIcon.UL * distScale * ScaleUI;
//}
//if (ScreenPos.Y < 0 + ViewportCenter.Y*EdgePercent)
//{
//	ScreenPos.Y = 0 + ViewportCenter.Y*EdgePercent;
//}
//if (ScreenPos.Y > ViewportSize.Y - ViewportCenter.Y*EdgePercent)// - ObjectiveIcon.UL * distScale * ScaleUI)
//{
//	ScreenPos.Y = ViewportSize.Y - ViewportCenter.Y*EdgePercent;// -ObjectiveIcon.VL * distScale * ScaleUI;
//}
////Canvas->SetDrawColor(BallIndicatorHUDColor);
////Canvas->DrawIcon(ObjectiveBallIcon, ScreenPos.X, ScreenPos.Y, ScaleUI * distScale);
//OutScreenPosition.X = ScreenPos.X;
//OutScreenPosition.Y = ScreenPos.Y;
//
////if (bLocationIsBehindCamera)
////{
////	// For behind the camera situation, we cheat a little to put the
////	// marker at the bottom of the screen so that it moves smoothly
////	// as you turn around. Could stand some refinement, but results
////	// are decent enough for most purposes.
//
////	FVector DiffVector = InLocation - PlayerCharacter->GetActorLocation();
////	FVector Inverted = DiffVector * -1.f;
////	FVector NewInLocation = PlayerCharacter->GetActorLocation() * Inverted;
//
////	NewInLocation.Z -= 5000;
//
////	PlayerController->ProjectWorldLocationToScreen(NewInLocation, OutScreenPosition);
////	OutScreenPosition.Y = (EdgePercent * ViewportCenter.X) * 2.f;
////	OutScreenPosition.X = -ViewportCenter.X - OutScreenPosition.X;
////}
//
////PlayerController->ProjectWorldLocationToScreen(InLocation, OutScreenPosition);//*ScreenPosition);
//
////																			  // Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.	
////if (OutScreenPosition.X >= 0.f && OutScreenPosition.X <= ViewportSize.X
////	&& OutScreenPosition.Y >= 0.f && OutScreenPosition.Y <= ViewportSize.Y)
////{
//
////	bIsOnScreen = true;
////	return;
////}
//
////OutScreenPosition -= ViewportCenter;
//
////float AngleRadians = FMath::Atan2(OutScreenPosition.Y, OutScreenPosition.X);
////AngleRadians -= FMath::DegreesToRadians(90.f);
//
////OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;
//
////float Cos = cosf(AngleRadians);
////float Sin = -sinf(AngleRadians);
//
////OutScreenPosition = FVector2D(ViewportCenter.X + (Sin * 180.f), ViewportCenter.Y + Cos * 180.f);
//
////float m = Cos / Sin;
//
////FVector2D ScreenBounds = ViewportCenter * EdgePercent;
//
////if (Cos > 0)
////{
////	OutScreenPosition = FVector2D(ScreenBounds.Y / m, ScreenBounds.Y);
////}
////else
////{
////	OutScreenPosition = FVector2D(-ScreenBounds.Y / m, -ScreenBounds.Y);
////}
//
////if (OutScreenPosition.X > ScreenBounds.X)
////{
////	OutScreenPosition = FVector2D(ScreenBounds.X, ScreenBounds.X*m);
////}
////else if (OutScreenPosition.X < -ScreenBounds.X)
////{
////	OutScreenPosition = FVector2D(-ScreenBounds.X, -ScreenBounds.X*m);
////}
//
////OutScreenPosition += ViewportCenter;